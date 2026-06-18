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

    uint64_t t{};           ///< 时间戳（纳秒）
    Eigen::Vector3d a_ib_b; ///< 原始加速度（m/s²）
    Eigen::Vector3d w_ib_b; ///< 原始角速度（rad/s）

    ImuMeasurement();

    /**
     * @brief Construct a new Imu Measurement object
     *
     * @param _t 时间戳（纳秒）
     * @param a_ib_b 加速度（m/s²）
     * @param w_ib_b 角速度（rad/s）
     */
    ImuMeasurement(const uint64_t _t, const Eigen::Vector3d &a_ib_b, const Eigen::Vector3d &w_ib_b);

    ~ImuMeasurement();

    friend std::ostream &operator<<(std::ostream &stream, const ImuMeasurement &meas);
};

} // namespace allan_variance
