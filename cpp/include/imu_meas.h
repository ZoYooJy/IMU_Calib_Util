#pragma once

#include <Eigen/Core>
#include <cstdint>
#include <string>

namespace allan_variance
{

/// IMU 单条测量数据
class ImuMeasurement
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    uint64_t t{};           ///< 时间戳（纳秒）
    Eigen::Vector3d I_a_WI; ///< 原始加速度（m/s²）
    Eigen::Vector3d I_w_WI; ///< 原始角速度（rad/s）

    /// 默认构造函数，加速度和角速度初始化为零
    ImuMeasurement();

    /// 带参数构造函数
    /// @param _t      时间戳（纳秒）
    /// @param _I_a_WI 加速度（m/s²）
    /// @param _I_w_WI 角速度（rad/s）
    ImuMeasurement(const uint64_t _t, const Eigen::Vector3d &_I_a_WI, const Eigen::Vector3d &_I_w_WI);

    /// 析构函数
    ~ImuMeasurement();

    /// 输出流操作符，打印 IMU 测量数据
    friend std::ostream &operator<<(std::ostream &stream, const ImuMeasurement &meas);
};

} // namespace allan_variance
