
/**
 * @file   data_reader.cc
 * @brief  IMU 数据文件读取器实现，从文本文件解析 $IMURAW 格式的 IMU 数据
 * @author Based on AllanVarianceComputor.cpp by Russell Buchanan
 */

#include "data_reader.h"
#include "debug.h"

#include <atomic>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>

namespace allan_variance
{

/// 全局运行标志定义
std::atomic<bool> g_running{true};

DataReader::DataReader(const std::string &imu_file, double imu_rate, int sequence_time, InputDataType data_type)
    : imu_file_(imu_file), imu_rate_(imu_rate), sequence_time_(sequence_time), data_type_(data_type)
{
}

bool DataReader::parseLineLSB(const std::string &line, ImuMeasurement &meas)
{
    // 检查行首是否为 "$IMURAW"
    if (line.size() < 7 || line.substr(0, 7) != "$IMURAW")
    {
        return false;
    }

    // 按逗号分割字段
    // 格式：$IMURAW, t, gyr_x, gyr_y, gyr_z, acc_x, acc_y, acc_z, temp*crc
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ','))
    {
        fields.push_back(field);
    }

    // 至少需要 8 个字段：$IMURAW, t, gyr_x, gyr_y, gyr_z, acc_x, acc_y, acc_z
    if (fields.size() < 8)
    {
        APP_ERROR("[WARN] Insufficient fields, skipping line: " << line);
        return false;
    }

    try
    {
        // 解析时间戳（秒）
        double t_sec = std::stod(fields[1]);

        // 解析陀螺仪比特值
        double gyr_x_bit = std::stod(fields[2]);
        double gyr_y_bit = std::stod(fields[3]);
        double gyr_z_bit = std::stod(fields[4]);

        // 解析加速度计比特值
        double acc_x_bit = std::stod(fields[5]);
        double acc_y_bit = std::stod(fields[6]);
        double acc_z_bit = std::stod(fields[7]);

        // 解析温度字段：temp*crc，取 '*' 前的数字部分
        // 温度值 = temp_bit * tempCoef + tempOffset
        // （当前未使用温度数据，仅解析以跳过该字段）

        // 比特值转物理量
        double gyr_x = gyr_x_bit * gyroCoef_; // rad/s
        double gyr_y = gyr_y_bit * gyroCoef_; // rad/s
        double gyr_z = gyr_z_bit * gyroCoef_; // rad/s
        double acc_x = acc_x_bit * accCoef_;  // m/s²
        double acc_y = acc_y_bit * accCoef_;  // m/s²
        double acc_z = acc_z_bit * accCoef_;  // m/s²

        // 构造 ImuMeasurement
        meas.t = s2ns(t_sec); // 秒转纳秒
        meas.a_ib_b = Eigen::Vector3d(acc_x, acc_y, acc_z);
        meas.w_ib_b = Eigen::Vector3d(gyr_x, gyr_y, gyr_z);

        return true;
    }
    catch (const std::exception &e)
    {
        APP_ERROR("[WARN] Failed to parse line: " << e.what() << ", content: " << line);
        return false;
    }
}

bool DataReader::parseLineNormal(const std::string &line, ImuMeasurement &meas)
{
    // 跳过空行和注释行
    if (line.empty() || line[0] == '#')
    {
        return false;
    }

    // 按空格分割字段
    // 格式：t, gyr_x, gyr_y, gyr_z, acc_x, acc_y, acc_z
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ' '))
    {
        fields.push_back(field);
    }

    // 需要 7 个字段：t, gyr_x, gyr_y, gyr_z, acc_x, acc_y, acc_z
    if (fields.size() < 7)
    {
        return false;
    }

    try
    {
        double t_sec = std::stod(fields[0]);

        // SI 格式直接是物理量，无需转换
        double gyr_x = std::stod(fields[1]);
        double gyr_y = std::stod(fields[2]);
        double gyr_z = std::stod(fields[3]);
        double acc_x = std::stod(fields[4]);
        double acc_y = std::stod(fields[5]);
        double acc_z = std::stod(fields[6]);

        meas.t = s2ns(t_sec);
        meas.a_ib_b = Eigen::Vector3d(acc_x, acc_y, acc_z);
        meas.w_ib_b = Eigen::Vector3d(gyr_x, gyr_y, gyr_z);

        return true;
    }
    catch (const std::exception &e)
    {
        APP_ERROR("[WARN] Failed to parse line: " << e.what() << ", content: " << line);
        return false;
    }
}

void DataReader::run(EigenVector<ImuMeasurement> &imuBuffer)
{
    std::ifstream file(imu_file_);
    if (!file.is_open())
    {
        APP_ERROR("[ERROR] Cannot open IMU data file: " << imu_file_);
        return;
    }

    int imu_counter = 0;      // 已读取的 IMU 消息计数
    int skipped_imu = 0;      // 因时间乱序被跳过的消息计数
    bool firstMsg = true;     // 是否为第一条消息
    uint64_t firstTime = 0;   // 第一条消息的时间戳（纳秒）
    uint64_t lastImuTime = 0; // 上一条消息的时间戳（纳秒）

    std::clock_t start = std::clock(); // 进度打印计时器
    std::string line;

    while (std::getline(file, line))
    {
        // 检查全局运行标志
        if (!g_running.load())
        {
            APP_ERROR("[ERROR] Stop signal received, closing file!");
            file.close();
            return;
        }

        ImuMeasurement meas;

        bool ok = (data_type_ == InputDataType::LSB) ? parseLineLSB(line, meas) : parseLineNormal(line, meas);
        if (!ok)
        {
            continue; // 非数据行或解析失败，跳过
        }

        imu_counter++;

        // 时长截断：sequence_time_ >= 0 时，超过此时长的数据不再读取；-1 表示读取全部数据
        double elapsed_seconds = ns2s(meas.t - (firstMsg ? meas.t : firstTime));
        if (!firstMsg && sequence_time_ >= 0 && elapsed_seconds > sequence_time_)
        {
            break;
        }

        // 每 60 秒打印一次进度
        if (std::difftime(std::clock(), start) / CLOCKS_PER_SEC >= 60.0)
        {
            double current_elapsed = firstMsg ? 0.0 : ns2s(meas.t - firstTime);
            APP_INFO("[INFO] Loaded " << current_elapsed << " / " << sequence_time_ << " s");
            start = std::clock();
        }

        // 首条消息初始化时间基准
        if (firstMsg)
        {
            firstMsg = false;
            firstTime = meas.t;
            lastImuTime = meas.t;
        }

        // 时间乱序检测
        if (meas.t < lastImuTime)
        {
            skipped_imu++;
            APP_ERROR("[ERROR] IMU timestamp out of order. Current(ns): " << meas.t - firstTime << " Previous(ns): "
                                                                          << lastImuTime - firstTime << " (dropped "
                                                                          << skipped_imu << ")");
            continue;
        }
        lastImuTime = meas.t;

        imuBuffer.push_back(meas);
    }

    file.close();

    APP_INFO("[INFO] Data collection done, " << imuBuffer.size() << " measurements loaded");
    if (skipped_imu > 0)
    {
        APP_ERROR("[WARN] Skipped " << skipped_imu << " messages due to out-of-order timestamps");
    }
}

} // namespace allan_variance