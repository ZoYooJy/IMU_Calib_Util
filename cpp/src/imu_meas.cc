/*
 * @Author: ZoYooJy zhouyujie_gdut@163.com
 * @Date: 2026-06-17 11:36:21
 * @LastEditors: ZoYooJy zhouyujie_gdut@163.com
 * @LastEditTime: 2026-06-18 15:45:19
 * @FilePath: /IMU_Calib_Util/cpp/src/imu_meas.cc
 */

#include "imu_meas.h"

namespace allan_variance
{

ImuMeasurement::ImuMeasurement() : a_ib_b_(Eigen::Vector3d::Zero()), w_ib_b_(Eigen::Vector3d::Zero())
{
}

ImuMeasurement::ImuMeasurement(const uint64_t _t, const Eigen::Vector3d &_a_ib_b, const Eigen::Vector3d &_w_ib_b)
{
    t_ = _t;
    a_ib_b_ = _a_ib_b;
    w_ib_b_ = _w_ib_b;
}


ImuMeasurement::~ImuMeasurement()
{
}

std::ostream &operator<<(std::ostream &stream, const ImuMeasurement &meas)
{
    stream << "IMU Measurement at time = " << meas.t_ << " : \n"
           << "a_ib_b: " << meas.a_ib_b_.transpose() << "\n"
           << "w_ib_b: " << meas.w_ib_b_.transpose() << "\n";
    return stream;
}

} // namespace allan_variance
