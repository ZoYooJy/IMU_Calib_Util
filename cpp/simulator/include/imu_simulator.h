/*
 * @Author: ZoYooJy zhouyujie_gdut@163.com
 * @Date: 2026-06-17 13:56:58
 * @LastEditors: ZoYooJy zhouyujie_gdut@163.com
 * @LastEditTime: 2026-06-18 15:36:39
 * @FilePath: /IMU_Calib_Util/cpp/simulator/include/imu_simulator.h
 */
#pragma once

#include <Eigen/Core>
#include <string>

namespace imu_simulator
{

/// 使用 Eigen 对齐分配器的 vector，避免固定大小 Eigen 向量的内存对齐问题
template <class T> using EigenVector = std::vector<T, Eigen::aligned_allocator<T>>;

/// IMU 仿真测量数据
struct SimImuMeasurement
{
    double t;             ///< 时间戳（秒）
    Eigen::Vector3d acc;  ///< 加速度（m/s²）
    Eigen::Vector3d gyro; ///< 角速度（rad/s）
    double temperature;   ///< 温度（摄氏度），仿真中固定为 25°C

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/// IMU 仿真器，基于 Kalibr IMU 噪声模型生成带噪声的 IMU 数据
/// 参考：https://github.com/ethz-asl/kalibr/wiki/IMU-Noise-Model
class ImuSimulator
{
  public:
    /**
     * @brief 从 YAML 配置文件加载噪声参数
     *
     *
     * @param config_file simu_config.yaml 文件路径
     */
    ImuSimulator(const std::string &config_file);

    /**
     * @brief 运行仿真，生成 IMU 数据并写入输出文件
     *
     */
    void run();

  private:
    /**
     * @brief 生成三维正态分布随机向量（各分量独立，标准差为 sigma）
     *
     * @param _sigma 各分量的标准差
     * @return 三维随机向量
     */
    Eigen::Vector3d randomNormalDistributionVector(double _sigma);

    /**
     * @brief 将一条仿真数据写入输出文件
     * 格式：t gyro_x gyro_y gyro_z acc_x acc_y acc_z, [t(s), gyro(rad/s), acc(m/s²)]
     *
     * @param _file 输出文件流
     * @param _meas 仿真测量数据
     */
    void writeMeasurement(std::ofstream &_file, const SimImuMeasurement &_meas);

    /// 加速度计噪声密度（白噪声标准差，m/s²/√Hz 或 m/s²·s^0.5）
    double accelerometer_noise_density_;
    /// 加速度计随机游走（零偏不稳定性驱动的游走，m/s²/√Hz 或 m/s³·√s）
    double accelerometer_random_walk_;
    /// 加速度计初始偏置（m/s²）
    double accelerometer_bias_init_;

    /// 陀螺仪噪声密度（白噪声标准差，rad/s/√Hz 或 rad/s·s^0.5）
    double gyroscope_noise_density_;
    /// 陀螺仪随机游走（零偏不稳定性驱动的游走，rad/s/√Hz 或 rad/s²·√s）
    double gyroscope_random_walk_;
    /// 陀螺仪初始偏置（rad/s）
    double gyroscope_bias_init_;

    /// IMU 采样频率（Hz）
    double update_rate_;
    /// 仿真时长（秒）
    double sequence_time_;
    /// 输出文件路径（目录）
    std::string output_path_;
};

} // namespace imu_simulator
