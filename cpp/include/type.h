/*
 * @Author: ZoYooJy zhouyujie_gdut@163.com
 * @Date: 2026-06-17 11:32:28
 * @LastEditors: ZoYooJy zhouyujie_gdut@163.com
 * @LastEditTime: 2026-06-25 17:31:22
 * @FilePath: /IMU_Calib_Util/cpp/include/type.h
 */
#pragma once

#include <Eigen/Core>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <map>
#include <vector>

namespace allan_variance
{

/// 秒转纳秒
inline uint64_t s2ns(double t)
{
    return static_cast<uint64_t>(t * 1000000000);
}

/// 纳秒转秒
inline double ns2s(uint64_t t)
{
    return static_cast<double>(t * 0.000000001);
}

/// 使用 Eigen 对齐分配器的 vector，避免固定大小 Eigen 向量的内存对齐问题
template <class T>
using EigenVector = std::vector<T, Eigen::aligned_allocator<T>>;

/// 原始 IMU 数据写入格式
struct ImuFormat
{
    double time;  ///< 时间戳（秒）
    double gyroX; ///< X 轴角速度（deg/s）
    double gyroY; ///< Y 轴角速度（deg/s）
    double gyroZ; ///< Z 轴角速度（deg/s）
    double accX;  ///< X 轴加速度（m/s²）
    double accY;  ///< Y 轴加速度（m/s²）
    double accZ;  ///< Z 轴加速度（m/s²）

    /// 将一条记录写入文件
    void writeOnFile(std::ofstream &file)
    {
        file << std::setprecision(19) << time << std::setprecision(7) << " " << accX << " " << accY << " " << accZ
             << " " << gyroX << " " << gyroY << " " << gyroZ << " " << std::endl;
    }
};

/// Allan 偏差结果写入格式
struct AllanVarianceFormat
{
    double period; ///< 采样周期 τ（秒）
    double gyroX;  ///< X 轴角速度 Allan 偏差
    double gyroY;  ///< Y 轴角速度 Allan 偏差
    double gyroZ;  ///< Z 轴角速度 Allan 偏差
    double accX;   ///< X 轴加速度 Allan 偏差
    double accY;   ///< Y 轴加速度 Allan 偏差
    double accZ;   ///< Z 轴加速度 Allan 偏差

    /// 将一条 Allan 偏差记录写入文件
    void writeOnFile(std::ofstream &file)
    {
        file << std::setprecision(19) << period << std::setprecision(7) << " " << accX << " " << accY << " " << accZ
             << " " << gyroX << " " << gyroY << " " << gyroZ << " " << std::endl;
    }
};

enum class InputDataType
{
    LSB,    //
    NORMAL, //
    BIN,    //
};

// clang-format off
const std::map<unsigned int, InputDataType> input_type_map = {
    {0, InputDataType::LSB},
    {1, InputDataType::NORMAL},
    {2, InputDataType::BIN}
};
// clang-format on

} // namespace allan_variance
