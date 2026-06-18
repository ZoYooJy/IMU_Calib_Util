/*
 * @Author: ZoYooJy zhouyujie_gdut@163.com
 * @Date: 2026-06-17 11:33:26
 * @LastEditors: ZoYooJy zhouyujie_gdut@163.com
 * @LastEditTime: 2026-06-18 15:47:28
 * @FilePath: /IMU_Calib_Util/cpp/include/yaml_parser.h
 */
#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <string>

#include <yaml-cpp/node/convert.h>
#include <yaml-cpp/node/detail/impl.h>
#include <yaml-cpp/node/node.h>

namespace allan_variance
{

/**
 * @brief 从 YAML 节点中安全读取参数，仅当键存在时更新值
 *
 * @tparam T
 * @param node YAML 节点
 * @param param 参数键名
 * @param value 输出值，键存在时被更新
 * @return true
 * @return false
 */
template <class T> static bool get(const YAML::Node &node, const std::string &param, T &value)
{
    if (!node[param])
    {
        return false;
    }
    value = node[param].as<T>();
    return true;
}

/**
 * @brief 加载 YAML 配置文件
 *
 * @param filename 文件路径
 * @return YAML 节点
 */
YAML::Node loadYamlFile(const std::string &_filename);

} // namespace allan_variance
