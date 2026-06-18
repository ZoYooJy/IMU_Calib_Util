/*
 * @Author: ZoYooJy zhouyujie_gdut@163.com
 * @Date: 2026-06-17 14:35:51
 * @LastEditors: ZoYooJy zhouyujie_gdut@163.com
 * @LastEditTime: 2026-06-18 14:13:36
 * @FilePath: /IMU_Calib_Util/cpp/src/yaml_parser.cc
 */
#include "yaml_parser.h"
#include "debug.h"

#include <yaml-cpp/node/parse.h>

namespace allan_variance
{

/**
 * @brief 加载 YAML 配置文件
 *
 * @param _filename 文件路径
 * @return YAML::Node YAML 节点
 */
YAML::Node loadYamlFile(const std::string &_filename)
{
    if (_filename.empty())
    {
        throw std::invalid_argument("Filename is empty!");
    }

    YAML::Node node;

    try
    {
        node = YAML::LoadFile(_filename);
    }
    catch (...)
    {
        throw std::invalid_argument("Error reading config file: " + _filename);
    }

    if (node.IsNull())
    {
        throw std::invalid_argument("Error reading config file: " + _filename);
    }

    APP_INFO("[INFO] Successfully loaded config file: " << _filename);

    return node;
}

} // namespace allan_variance
