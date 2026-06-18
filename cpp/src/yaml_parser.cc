/*
 * @Author: ZoYooJy zhouyujie_gdut@163.com
 * @Date: 2026-06-17 14:35:51
 * @LastEditors: ZoYooJy zhouyujie_gdut@163.com
 * @LastEditTime: 2026-06-18 15:47:59
 * @FilePath: /IMU_Calib_Util/cpp/src/yaml_parser.cc
 */
#include "yaml_parser.h"
#include "debug.h"

#include <yaml-cpp/node/parse.h>

namespace allan_variance
{

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
