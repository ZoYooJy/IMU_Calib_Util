/*
 * @Author: ZoYooJy zhouyujie_gdut@163.com
 * @Date: 2026-06-17 13:57:09
 * @LastEditors: ZoYooJy zhouyujie_gdut@163.com
 * @LastEditTime: 2026-06-17 18:14:36
 * @FilePath: /IMU_Calib_Util/cpp/simulator/src/main.cc
 */

#include <csignal>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <string>

#include "imu_simulator.h"

namespace fs = std::filesystem;

/// 全局运行标志，用于信号处理优雅退出
static bool g_running = true;

/// 信号处理函数，捕获 SIGINT (Ctrl+C) 等信号
/// @param signum 信号编号
void signalHandler(int signum)
{
    std::cout << "[INFO] 收到信号 " << signum << "，正在停止..." << std::endl;
    g_running = false;
}

int main(int argc, char **argv)
{
    // 注册信号处理器
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // 检查命令行参数：argv[1] 为 simu_config.yaml 路径
    if (argc < 2)
    {
        std::cerr << "[ERROR] 用法: " << argv[0] << " <simu_config.yaml>" << std::endl;
        return 1;
    }

    std::string config_file = argv[1];
    std::cout << "[INFO] 配置文件: " << config_file << std::endl;

    // 创建 IMU 仿真器并运行
    imu_simulator::ImuSimulator simulator(config_file);
    std::cout << "[INFO] IMU 仿真器已初始化" << std::endl;

    std::clock_t start = std::clock();
    simulator.run();
    double durationTime = (std::clock() - start) / static_cast<double>(CLOCKS_PER_SEC);

    std::cout << "[INFO] 总计算时间: " << durationTime << " s" << std::endl;

    return 0;
}
