
/**
 * @file   allan_variance_computor.cc
 * @brief  Allan 方差计算器实现
 * @author Russell Buchanan (original), modified for non-ROS version
 */

#include "allan_variance_computor.h"
#include "data_reader.h"

#include <atomic>
#include <cmath>
#include <ctime>
#include <future>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "debug.h"

namespace allan_variance
{

AllanVarianceComputor::AllanVarianceComputor(const std::string &config_file, const std::string &output_path)
    : overlap_(0.0)
{
    // 加载 YAML 配置文件
    YAML::Node node = loadYamlFile(config_file);

    int data_type;

    // 读取配置参数
    get(node, "imu_rate", imu_rate_);
    get(node, "sequence_time", sequence_time_);
    get(node, "overlap", overlap_);
    get(node, "data_type", data_type);

    if (input_type_map.find(data_type) != input_type_map.end())
    {
        data_type_ = input_type_map.at(data_type);
    }
    else
    {
        data_type_ = InputDataType::LSB; // default
    }

    // 设置输出文件路径
    imu_output_file_ = output_path + "/" + "allan_variance" + ".csv";

    APP_INFO("[INFO] imu_rate: " << imu_rate_);
    APP_INFO("[INFO] sequence_time: " << sequence_time_);
    APP_INFO("[INFO] overlap: " << overlap_);
    APP_INFO("[INFO] data_type: " << data_type);
    APP_INFO("[INFO] output file path: " << imu_output_file_);
}

void AllanVarianceComputor::run(const std::string &imu_file)
{
    av_output_ = std::ofstream(imu_output_file_.c_str(), std::ofstream::out);

    DataReader reader(imu_file, imu_rate_, sequence_time_, data_type_);
    reader.run(imuBuffer_);

    // 计算 Allan 方差
    if (!imuBuffer_.empty())
    {
        allanVariance();
    }
    else
    {
        APP_ERROR("[ERROR] No IMU data to process, check file path and format");
    }
}

void AllanVarianceComputor::closeOutputs()
{
    av_output_.close();
}

void AllanVarianceComputor::allanVariance()
{

    std::mutex mtx;                                               // 保护 averages_map 的互斥锁
    std::map<int, std::vector<std::vector<double>>> averages_map; // 各周期对应的 bin 均值

    // 采样周期范围：0.1s 到 1000s（period=1~10000，乘以 0.1）
    int period_min = 1;
    int period_max = 10000;

    // 使用 std::async 并行计算各周期的 bin 均值（替代 OpenMP）
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0)
        num_threads = 4; // 若无法检测核心数，默认 4 线程

    // 将 period 范围拆分为多个任务块
    int total_periods = period_max - period_min;
    int chunk_size = total_periods / num_threads;
    if (chunk_size == 0)
        chunk_size = 1;

    // 存储各线程的 future
    std::vector<std::future<std::map<int, std::vector<std::vector<double>>>>> futures;

    for (unsigned int t = 0; t < num_threads; ++t)
    {
        int start_period = period_min + t * chunk_size;
        int end_period = (t == num_threads - 1) ? period_max : start_period + chunk_size;

        // 确保范围有效
        if (start_period >= period_max)
            break;

        // 捕获所需变量（imuBuffer_ 为只读，无需拷贝）
        futures.push_back(std::async(
            std::launch::async,
            [start_period, end_period, period_max, this]() -> std::map<int, std::vector<std::vector<double>>> {
                std::map<int, std::vector<std::vector<double>>> local_map;

                for (int period = start_period; period < end_period; period++)
                {
                    // 检查全局运行标志
                    if (!g_running.load())
                        break;

                    std::vector<std::vector<double>> averages;
                    double period_time = period * 0.1; // 采样周期（秒）

                    int max_bin_size = static_cast<int>(period_time * imu_rate_);        // 每个 bin 包含的测量数
                    int overlap = static_cast<int>(std::floor(max_bin_size * overlap_)); // 重叠量

                    std::vector<double> current_average = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

                    // 计算 bin 均值（重叠法）
                    for (int j = 0; j < (static_cast<int>(imuBuffer_.size()) - max_bin_size);
                         j += (max_bin_size - overlap))
                    {
                        // 对当前 bin 内所有测量求和
                        for (int m = 0; m < max_bin_size; m++)
                        {
                            // 加速度（m/s²）
                            current_average[0] += imuBuffer_[j + m].a_ib_b_[0];
                            current_average[1] += imuBuffer_[j + m].a_ib_b_[1];
                            current_average[2] += imuBuffer_[j + m].a_ib_b_[2];

                            // 角速度（rad/s）
                            current_average[3] += imuBuffer_[j + m].w_ib_b_[0];
                            current_average[4] += imuBuffer_[j + m].w_ib_b_[1];
                            current_average[5] += imuBuffer_[j + m].w_ib_b_[2];
                        }

                        // 求均值
                        current_average[0] /= max_bin_size;
                        current_average[1] /= max_bin_size;
                        current_average[2] /= max_bin_size;
                        current_average[3] /= max_bin_size;
                        current_average[4] /= max_bin_size;
                        current_average[5] /= max_bin_size;

                        averages.push_back(current_average);
                        current_average = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
                    }

                    int num_averages = static_cast<int>(averages.size());
                    APP_INFO("[INFO] Period " << period_time << "s: computed " << num_averages << " bin averages ("
                                              << (period_max - period - 1) << " periods remaining)");

                    local_map.insert({period, averages});
                }

                return local_map;
            }));
    }

    // 等待所有线程完成，合并结果
    for (auto &f : futures)
    {
        auto local_map = f.get();
        for (auto &[key, val] : local_map)
        {
            averages_map.insert({key, std::move(val)});
        }
    }

    // 检查是否被信号中断
    if (!g_running.load())
    {
        APP_ERROR("[ERROR] Stop signal received, aborting computation!");
        return;
    }

    // 计算 Allan 方差和 Allan 偏差
    std::vector<std::vector<double>> allan_variances;
    for (int period = period_min; period < period_max; period++)
    {
        // 跳过未计算的周期（可能因信号中断）
        if (averages_map.find(period) == averages_map.end())
        {
            continue;
        }

        std::vector<std::vector<double>> averages = averages_map.at(period);
        double period_time = period * 0.1; // 采样周期（秒）
        int num_averages = static_cast<int>(averages.size());
        APP_INFO("[INFO] Period " << period_time << "s: " << num_averages << " bins, " << imuBuffer_.size()
                                  << " measurements total");

        // 计算 Allan 方差：σ²(τ) = 1/(2(N-1)) * Σ(ā_{k+1} - ā_k)²
        std::vector<double> allan_variance = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        for (int k = 0; k < num_averages - 1; k++)
        {
            allan_variance[0] += std::pow(averages[k + 1][0] - averages[k][0], 2);
            allan_variance[1] += std::pow(averages[k + 1][1] - averages[k][1], 2);
            allan_variance[2] += std::pow(averages[k + 1][2] - averages[k][2], 2);
            allan_variance[3] += std::pow(averages[k + 1][3] - averages[k][3], 2);
            allan_variance[4] += std::pow(averages[k + 1][4] - averages[k][4], 2);
            allan_variance[5] += std::pow(averages[k + 1][5] - averages[k][5], 2);
        }

        // Allan 方差
        std::vector<double> avar = {
            allan_variance[0] / (2 * (num_averages - 1)), allan_variance[1] / (2 * (num_averages - 1)),
            allan_variance[2] / (2 * (num_averages - 1)), allan_variance[3] / (2 * (num_averages - 1)),
            allan_variance[4] / (2 * (num_averages - 1)), allan_variance[5] / (2 * (num_averages - 1))};

        // Allan 偏差 = sqrt(Allan 方差)
        std::vector<double> allan_deviation = {std::sqrt(avar[0]), std::sqrt(avar[1]), std::sqrt(avar[2]),
                                               std::sqrt(avar[3]), std::sqrt(avar[4]), std::sqrt(avar[5])};

        // 写入文件
        writeAllanDeviation(allan_deviation, period_time);

        allan_variances.push_back(avar);
    }
}

void AllanVarianceComputor::writeAllanDeviation(std::vector<double> variance, double period)
{
    aVRecorder_.period = period;
    aVRecorder_.accX = variance[0];
    aVRecorder_.accY = variance[1];
    aVRecorder_.accZ = variance[2];
    aVRecorder_.gyroX = variance[3];
    aVRecorder_.gyroY = variance[4];
    aVRecorder_.gyroZ = variance[5];
    aVRecorder_.writeOnFile(av_output_);
}

} // namespace allan_variance
