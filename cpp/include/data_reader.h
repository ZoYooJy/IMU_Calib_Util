/*
 * @Author: ZoYooJy zhouyujie_gdut@163.com
 * @Date: 2026-06-17 11:34:58
 * @LastEditors: ZoYooJy zhouyujie_gdut@163.com
 * @LastEditTime: 2026-06-17 11:35:05
 * @FilePath: /IMU_Calib_Util/cpp/include/data_reader.h
 */
#pragma once

#include <atomic>
#include <string>

#include "imu_meas.h"
#include "type.h"

namespace allan_variance
{

/// 全局运行标志，由信号处理器控制，用于优雅退出
extern std::atomic<bool> g_running;

/// IMU 数据文件读取器，从文本文件解析 $IMURAW 格式的 IMU 数据
class DataReader
{
  public:
    /// 构造函数
    /// @param imu_file      IMU 数据文件路径
    /// @param imu_rate      IMU 采样频率（Hz），用于进度打印
    /// @param sequence_time 数据时长截断（秒），超过此时长的数据将被忽略
    DataReader(const std::string &imu_file, double imu_rate, int sequence_time);

    /// 读取 IMU 数据文件，将解析结果填充到 imuBuffer
    /// @param imuBuffer 输出的 IMU 测量数据缓冲区
    void run(EigenVector<ImuMeasurement> &imuBuffer);

  private:
    /// 解析单行 IMU 数据
    /// 格式：$IMURAW, t, gyr_x, gyr_y, gyr_z, acc_x, acc_y, acc_z, temp*crc
    /// @param line 输入行字符串
    /// @param meas 输出的 IMU 测量数据
    /// @return 是否解析成功
    bool parseLine(const std::string &line, ImuMeasurement &meas);

    std::string imu_file_; ///< IMU 数据文件路径
    double imu_rate_;      ///< IMU 采样频率（Hz）
    int sequence_time_;    ///< 数据时长截断（秒）

    /// 陀螺仪比特值转 rad/s 的系数：gyroRange * PI / 180 / N
    static constexpr double gyroCoef_ = 0.000266;
    /// 加速度计比特值转 m/s² 的系数：accRange * G0 / N
    static constexpr double accCoef_ = 0.000598;
    /// 温度比特值系数：1.0 / (1 << 9)
    static constexpr double tempCoef_ = 0.001953;
    /// 温度偏移量（摄氏度）
    static constexpr double tempOffset_ = 23.0;
};

} // namespace allan_variance
