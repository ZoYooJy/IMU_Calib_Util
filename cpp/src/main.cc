
/**
 * @file   main.cc
 * @brief  Allan 方差计算工具主入口
 * @author Russell Buchanan (original), modified for non-ROS version
 */

#include <atomic>
#include <csignal>
#include <ctime>
#include <filesystem>
#include <string>

#include "allan_variance_computor.h"
#include "data_reader.h"
#include "debug.h"
#include "yaml_parser.h"

namespace fs = std::filesystem;

// g_running 定义在 data_reader.cc 中，此处通过 data_reader.h 的 extern 声明引用

/// 信号处理函数，捕获 SIGINT (Ctrl+C) 等信号，设置运行标志为 false
/// @param signum 信号编号
void signalHandler(int signum)
{
    APP_INFO("[INFO] Received signal " << signum << ", stopping...");
    allan_variance::g_running.store(false);
}

int main(int argc, char **argv)
{
    // 注册信号处理器
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // 检查命令行参数：argv[1] 为 config.yaml 路径
    if (argc < 2)
    {
        APP_ERROR("[ERROR] Usage: " << argv[0] << " <config.yaml>");
        return 1;
    }

    std::string config_file = argv[1];
    APP_INFO("[INFO] Config file: " << config_file);

    // 加载配置文件
    YAML::Node config;
    try
    {
        config = allan_variance::loadYamlFile(config_file);
    }
    catch (const std::exception &e)
    {
        APP_ERROR("[ERROR] Failed to read config file: " << e.what());
        return 1;
    }

    // 从配置文件读取参数
    std::string imu_file;
    std::string output_path;

    allan_variance::get(config, "imu_file", imu_file);
    allan_variance::get(config, "output_path", output_path);

    if (imu_file.empty())
    {
        APP_ERROR("[ERROR] imu_file not specified in config");
        return 1;
    }
    if (output_path.empty())
    {
        output_path = ""; // TODO 默认输出到与imu_file相同的目录
    }

    // 确保输出目录存在
    fs::path out_dir = fs::absolute(fs::path(output_path));
    if (!fs::exists(out_dir))
    {
        APP_INFO("[INFO] Creating output directory: " << out_dir.string());
        fs::create_directories(out_dir);
    }

    APP_INFO("[INFO] IMU data file: " << imu_file);
    APP_INFO("[INFO] Output path: " << out_dir.string());

    // 创建 Allan 方差计算器并执行计算
    allan_variance::AllanVarianceComputor computor(config_file, out_dir.string());
    APP_INFO("[INFO] Allan variance computor initialized");

    std::clock_t start = std::clock();
    computor.run(imu_file);
    double durationTime = (std::clock() - start) / static_cast<double>(CLOCKS_PER_SEC);

    APP_INFO("[INFO] Total computation time: " << durationTime << " s");
    APP_INFO("[INFO] Data written to allan_variance.csv");

    return 0;
}
