#include "debug.h"
#include "yaml_parser.h"

#include <yaml-cpp/node/parse.h>

namespace allan_variance
{

/// 加载 YAML 配置文件
/// @param filename 文件路径
/// @return YAML 节点
YAML::Node loadYamlFile(const std::string &filename)
{
    if (filename.empty())
    {
        throw std::invalid_argument("Filename is empty!");
    }

    YAML::Node node;

    try
    {
        node = YAML::LoadFile(filename);
    }
    catch (...)
    {
        throw std::invalid_argument("Error reading config file: " + filename);
    }

    if (node.IsNull())
    {
        throw std::invalid_argument("Error reading config file: " + filename);
    }

    APP_INFO("[INFO] Successfully loaded config file: " << filename);

    return node;
}

} // namespace allan_variance
