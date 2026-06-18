/*
 * @Author: ZoYooJy zhouyujie_gdut@163.com
 * @Date: 2026-06-17 11:33:42
 * @LastEditors: ZoYooJy zhouyujie_gdut@163.com
 * @LastEditTime: 2026-06-18 15:45:26
 * @FilePath: /IMU_Calib_Util/cpp/include/imu_meas.h
 */
#pragma once

#include <Eigen/Core>
#include <cstdint>
#include <string>

namespace allan_variance
{

class ImuMeasurement
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    uint64_t t_{};           ///< 时间戳（纳秒）
    Eigen::Vector3d a_ib_b_; ///< 原始加速度（m/s²）
    Eigen::Vector3d w_ib_b_; ///< 原始角速度（rad/s）

    ImuMeasurement();

    /**
     * @brief Construct a new Imu Measurement object
     *
     * @param _t 时间戳（纳秒）
     * @param _a_ib_b 加速度（m/s²）
     * @param _w_ib_b 角速度（rad/s）
     */
    ImuMeasurement(const uint64_t _t, const Eigen::Vector3d &_a_ib_b, const Eigen::Vector3d &_w_ib_b);

    ~ImuMeasurement();

    /**
     * @brief 输出流操作符，打印 IMU 测量数据
     *
     * @param stream
     * @param meas
     * @return
     */
    friend std::ostream &operator<<(std::ostream &stream, const ImuMeasurement &meas);
};

} // namespace allan_variance
