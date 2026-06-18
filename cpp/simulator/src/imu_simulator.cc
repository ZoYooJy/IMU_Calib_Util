
/**
 * @file   imu_simulator.cc
 * @brief  IMU 仿真器实现，基于 Kalibr IMU 噪声模型生成带噪声的 IMU 数据
 * @author Based on ImuSimulator.cpp by Rick Liu, modified for non-ROS version
 */

#include "imu_simulator.h"

#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

#include <yaml-cpp/yaml.h>

namespace imu_simulator
{

/// 从 YAML 节点中安全读取参数，仅当键存在时更新值
/// @param node  YAML 节点
/// @param param 参数键名
/// @param value 输出值，键存在时被更新
/// @return 是否成功读取
template <class T> static bool get(const YAML::Node &node, const std::string &param, T &value)
{
    if (!node[param])
    {
        return false;
    }
    value = node[param].as<T>();
    return true;
}

/// 加载 YAML 配置文件
/// @param filename 文件路径
/// @return YAML 节点
static YAML::Node loadYamlFile(const std::string &filename)
{
    if (filename.empty())
    {
        throw std::invalid_argument("Filename is empty!");
    }

    YAML::Node node;
    try
    {
        node = YAML::LoadFile(filename);
    }
    catch (...)
    {
        throw std::invalid_argument("Error reading config file: " + filename);
    }

    if (node.IsNull())
    {
        throw std::invalid_argument("Error reading config file: " + filename);
    }

    std::cout << "[INFO] 成功读取配置文件: " << filename << std::endl;
    return node;
}

ImuSimulator::ImuSimulator(const std::string &config_file)
{
    // 加载 YAML 配置文件
    YAML::Node config = loadYamlFile(config_file);

    // 读取加速度计噪声参数
    get(config, "accelerometer_noise_density", accelerometer_noise_density_);
    std::cout << "[INFO] accelerometer_noise_density: " << accelerometer_noise_density_ << std::endl;
    get(config, "accelerometer_random_walk", accelerometer_random_walk_);
    std::cout << "[INFO] accelerometer_random_walk: " << accelerometer_random_walk_ << std::endl;
    get(config, "accelerometer_bias_init", accelerometer_bias_init_);
    std::cout << "[INFO] accelerometer_bias_init: " << accelerometer_bias_init_ << std::endl;

    // 读取陀螺仪噪声参数
    get(config, "gyroscope_noise_density", gyroscope_noise_density_);
    std::cout << "[INFO] gyroscope_noise_density: " << gyroscope_noise_density_ << std::endl;
    get(config, "gyroscope_random_walk", gyroscope_random_walk_);
    std::cout << "[INFO] gyroscope_random_walk: " << gyroscope_random_walk_ << std::endl;
    get(config, "gyroscope_bias_init", gyroscope_bias_init_);
    std::cout << "[INFO] gyroscope_bias_init: " << gyroscope_bias_init_ << std::endl;

    // 读取仿真参数
    get(config, "update_rate", update_rate_);
    std::cout << "[INFO] update_rate: " << update_rate_ << std::endl;
    get(config, "sequence_time", sequence_time_);
    std::cout << "[INFO] sequence_time: " << sequence_time_ << std::endl;

    // 读取输出路径
    get(config, "output_path", output_path_);
    if (output_path_.empty())
    {
        output_path_ = "."; // 默认输出到当前目录
    }
    std::cout << "[INFO] output_path: " << output_path_ << std::endl;
}

Eigen::Vector3d ImuSimulator::randomNormalDistributionVector(double sigma)
{
    // 使用 C++11 随机数引擎，thread_local 保证线程安全且避免重复构造
    thread_local std::mt19937 rng{std::random_device{}()};
    thread_local std::normal_distribution<double> nd{0.0, 1.0};

    return {sigma * nd(rng), sigma * nd(rng), sigma * nd(rng)};
}

void ImuSimulator::writeMeasurement(std::ofstream &file, const SimImuMeasurement &meas)
{
    // 格式：t gyro_x gyro_y gyro_z acc_x acc_y acc_z
    // 单位：t(s), gyro(rad/s), acc(m/s²)
    file << std::setprecision(19) << meas.t << " " << meas.gyro.x() << " " << meas.gyro.y() << " " << meas.gyro.z()
         << " " << meas.acc.x() << " " << meas.acc.y() << " " << meas.acc.z() << std::endl;
}

void ImuSimulator::run()
{
    std::cout << "[INFO] 开始生成 IMU 仿真数据 ..." << std::endl;

    // 构造输出文件路径
    std::string output_file = output_path_ + "/imu_simulate.txt";
    std::ofstream file(output_file, std::ofstream::out);
    if (!file.is_open())
    {
        std::cerr << "[ERROR] 无法创建输出文件: " << output_file << std::endl;
        return;
    }

    // 采样时间间隔（秒）
    double dt = 1.0 / update_rate_;

    // 初始化偏置：各轴偏置初始值相同
    Eigen::Vector3d accelerometer_bias = Eigen::Vector3d::Constant(accelerometer_bias_init_);
    Eigen::Vector3d gyroscope_bias = Eigen::Vector3d::Constant(gyroscope_bias_init_);

    // 真实加速度和角速度（静态仿真，均为零）
    Eigen::Vector3d accelerometer_real = Eigen::Vector3d::Zero();
    Eigen::Vector3d gyroscope_real = Eigen::Vector3d::Zero();

    // 仿真温度（固定 25°C）
    double temperature = 25.0;

    // 总采样点数
    int64_t total_samples = static_cast<int64_t>(sequence_time_ * update_rate_);

    // 进度打印计时器
    std::clock_t start = std::clock();

    for (int64_t i = 0; i < total_samples; ++i)
    {
        // 当前时间戳（秒）
        double t = i * dt;

        // 每 2 秒打印一次进度
        if (std::difftime(std::clock(), start) / CLOCKS_PER_SEC >= 2.0)
        {
            std::cout << "[INFO] 已生成 " << t << " / " << sequence_time_ << " 秒" << std::endl;
            start = std::clock();
        }

        // Kalibr IMU 噪声模型：
        // 偏置随机游走：b(t+dt) = b(t) + n_w * sqrt(dt)，n_w ~ N(0, sigma_rw)
        accelerometer_bias += randomNormalDistributionVector(accelerometer_random_walk_) * std::sqrt(dt);
        gyroscope_bias += randomNormalDistributionVector(gyroscope_random_walk_) * std::sqrt(dt);

        // 测量值 = 真实值 + 偏置 + 白噪声，白噪声 n_d ~ N(0, sigma_nd) / sqrt(dt)
        Eigen::Vector3d acc_measure = accelerometer_real + accelerometer_bias +
                                      randomNormalDistributionVector(accelerometer_noise_density_) / std::sqrt(dt);
        Eigen::Vector3d gyro_measure =
            gyroscope_real + gyroscope_bias + randomNormalDistributionVector(gyroscope_noise_density_) / std::sqrt(dt);

        // 构造仿真测量数据
        SimImuMeasurement meas;
        meas.t = t;
        meas.acc = acc_measure;
        meas.gyro = gyro_measure;
        meas.temperature = temperature;

        // 写入文件
        writeMeasurement(file, meas);
    }

    file.close();

    std::cout << "[INFO] 仿真数据生成完成，共 " << total_samples << " 条数据" << std::endl;
    std::cout << "[INFO] 输出文件: " << output_file << std::endl;
}

} // namespace imu_simulator
