#pragma once

#include <string>

#include "imu_meas.h"
#include "type.h"
#include "yaml_parser.h"

namespace allan_variance
{

/// Allan 方差计算器，从 IMU 数据计算 Allan 方差和 Allan 偏差
class AllanVarianceComputor
{
  public:
    /// 构造函数
    /// @param config_file YAML 配置文件路径
    /// @param output_path 结果输出目录路径
    AllanVarianceComputor(const std::string &config_file, const std::string &output_path);

    /// 析构函数，关闭输出文件
    virtual ~AllanVarianceComputor()
    {
        closeOutputs();
    }

    /// 执行完整的 Allan 方差计算流程：读取数据 → 计算方差 → 写入文件
    /// @param imu_file IMU 数据文件路径
    void run(const std::string &imu_file);

    /// 关闭输出文件流
    void closeOutputs();

  private:
    /// 计算 Allan 方差（核心算法）
    void allanVariance();

    /// 写入一行 Allan 偏差结果到 CSV 文件
    /// @param variance 六轴 Allan 偏差值 {accX, accY, accZ, gyroX, gyroY, gyroZ}
    /// @param period   采样周期 τ（秒）
    void writeAllanDeviation(std::vector<double> variance, double period);

    // --- 数据 ---
    AllanVarianceFormat aVRecorder_{};      ///< Allan 偏差记录器，用于格式化写入
    std::ofstream av_output_;               ///< 输出文件流
    std::string imu_output_file_;           ///< 输出 CSV 文件路径
    EigenVector<ImuMeasurement> imuBuffer_; ///< IMU 数据缓冲区

    // --- 配置 ---
    int sequence_time_{};     ///< 数据时长截断（秒）
    double imu_rate_ = 100.0; ///< IMU 采样频率（Hz）
    float overlap_ = 0.0;     ///< bin 重叠百分比（0.0=标准法，>0=重叠法）
};

} // namespace allan_variance
