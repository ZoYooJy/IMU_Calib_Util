
#include "imu_meas.h"

namespace allan_variance
{

/// 默认构造函数，加速度和角速度初始化为零
ImuMeasurement::ImuMeasurement() : a_ib_b(0, 0, 0), w_ib_b(0, 0, 0)
{
}

/// 带参数构造函数
ImuMeasurement::ImuMeasurement(const uint64_t _t, const Eigen::Vector3d &_I_a_WI, const Eigen::Vector3d &_I_w_WI)
{
    t = _t;
    a_ib_b = _I_a_WI;
    w_ib_b = _I_w_WI;
}

/// 析构函数
ImuMeasurement::~ImuMeasurement()
{
}

/// 输出流操作符，打印 IMU 测量数据
std::ostream &operator<<(std::ostream &stream, const ImuMeasurement &meas)
{
    stream << "IMU Measurement at time = " << meas.t << " : \n"
           << "a_ib_b: " << meas.a_ib_b.transpose() << "\n"
           << "w_ib_b: " << meas.w_ib_b.transpose() << "\n";
    return stream;
}

} // namespace allan_variance
