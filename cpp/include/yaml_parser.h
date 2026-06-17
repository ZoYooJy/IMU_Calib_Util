#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <string>

#include <yaml-cpp/node/convert.h>
#include <yaml-cpp/node/detail/impl.h>
#include <yaml-cpp/node/node.h>

namespace allan_variance
{

/// 从 YAML 节点中安全读取参数，仅当键存在时更新值
/// @param node  YAML 节点
/// @param param 参数键名
/// @param value 输出值，键存在时被更新
/// @return 是否成功读取
template <class T> static bool get(const YAML::Node &node, const std::string &param, T &value)
{
    if (!node[param])
    {
        return false;
    }
    value = node[param].as<T>();
    return true;
}

/// 加载 YAML 配置文件
/// @param filename 文件路径
/// @return YAML 节点
YAML::Node loadYamlFile(const std::string &filename);

} // namespace allan_variance
