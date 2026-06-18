/*
 * @Author: ZoYooJy zhouyujie_gdut@163.com
 * @Date: 2026-06-17 11:34:58
 * @LastEditors: ZoYooJy zhouyujie_gdut@163.com
 * @LastEditTime: 2026-06-18 11:26:43
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
    /**
     * @brief
     *
     * @param _imu_file IMU 数据文件路径
     * @param _imu_rate IMU 采样频率（Hz）
     * @param _sequence_time 数据时长截断（秒），超过此时长的数据将被忽略
     * @param _data_type 输入数据的格式
     */
    DataReader(const std::string &_imu_file, double _imu_rate, int _sequence_time, InputDataType _data_type);

    /**
     * @brief 读取 IMU 数据文件，将解析结果填充到 imuBuffer
     *
     * @param _imuBuffer 输出的 IMU 测量数据缓冲区
     */
    void run(EigenVector<ImuMeasurement> &_imuBuffer);

  private:
    /**
     * @brief 解析单行 IMU 数据
     * 格式：$IMURAW, t, gyr_x, gyr_y, gyr_z, acc_x, acc_y, acc_z, temp*crc [LSB]
     *
     * @param _line 输入行字符串
     * @param _meas 输出的 IMU 测量数据
     * @return 是否解析成功
     */
    bool parseLineLSB(const std::string &_line, ImuMeasurement &_meas);

    /**
     * @brief 解析单行 IMU 数据
     * 格式: t, gyr_x, gyr_y, gyr_z, acc_x, acc_y, acc_z [rad/s, m/s^2]
     *
     * @param _line 输入行字符串
     * @param _meas 输出的 IMU 测量数据
     * @return 是否解析成功
     */
    bool parseLineNormal(const std::string &_line, ImuMeasurement &_meas);

    std::string imu_file_; ///< IMU 数据文件路径
    double imu_rate_;      ///< IMU 采样频率（Hz）
    int sequence_time_;    ///< 数据时长截断（秒）

    InputDataType data_type_; //

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
