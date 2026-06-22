<!--
 * @Author: ZoYooJy zhouyujie_gdut@163.com
 * @Date: 2026-06-17 16:42:27
 * @LastEditors: ZoYooJy zhouyujie_gdut@163.com
 * @LastEditTime: 2026-06-22 10:33:28
 * @FilePath: /IMU_Calib_Util/README.md
-->

# IMU Allan-Variance calibration without ROS.

## 项目概述

IMU Allan方差标定工具（无ROS依赖）。从静态IMU数据计算Allan方差/偏差，拟合噪声参数（白噪声密度、随机游走、零偏不稳定性），并生成图表。基于 [allan_variance_ros](https://github.com/ori-drs/allan_variance_ros)。

## 架构

**`cpp/` — Allan方差计算器**（命名空间 `allan_variance`）
- `main.cc` → 加载YAML配置，创建 `AllanVarianceComputor`，运行计算流程
- `AllanVarianceComputor` → 通过 `DataReader` 读取IMU数据，多线程bin均值计算Allan方差（`std::async`），输出 `allan_variance.csv`
- `DataReader` → 解析两种IMU数据格式：LSB格式（`$IMURAW` 逗号分隔，含比特值到SI单位转换）和Normal格式（空格分隔的SI单位）。由配置中 `data_type` 控制（1=LSB, 2=Normal）
- `ImuMeasurement` → 时间戳(ns) + 加速度(m/s²) + 角速度(rad/s)
- `yaml_parser` → 安全YAML键值读取，`get<T>()` 模板函数

**`cpp/simulator/` — IMU数据仿真生成器**（命名空间 `imu_simulator`）
- 基于Kalibr噪声模型（零偏随机游走 + 白噪声）生成静态IMU仿真数据
- 输出格式：空格分隔 `t gyro_x gyro_y gyro_z acc_x acc_y acc_z`（SI单位，兼容data_type=2）
- 配置项包括噪声密度、随机游走系数、初始偏置、采样率、仿真时长

**`py/`** — python版本的代码实现


**`scripts/analysis.py`** — 后处理
- 读取 `allan_variance.csv`，拟合白噪声线（斜率-0.5，τ=1处）和随机游走线（斜率+0.5，τ=3处）
- 输出各轴噪声密度、随机游走、零偏不稳定性及均值
- 生成对数坐标Allan偏差曲线图，叠加拟合模型

**`config/`** — 配置文件
- `config.yaml` — 真实传感器数据配置（LSB格式，例：bmi323_static.DAT）
- `simu_config.yaml` — 仿真器输出作为Allan方差计算器输入（Normal格式）
- `imu_generate.yaml` — 仿真器噪声参数和输出路径


## 构建与运行命令

### Allan方差计算器
```bash
cd cpp/build && cmake .. && make
./allan_variance_calib <config.yaml>
# 示例：
./allan_variance_calib ../../config/config.yaml
```

### IMU仿真数据生成器
```bash
cd cpp/simulator/build && cmake .. && make
./imu_simulator <simu_config.yaml>
# 示例：
./imu_simulator ../../../config/imu_generate.yaml
```

### Python分析脚本（绘图 + 参数拟合）
```bash
python3 scripts/analysis.py --cfg scripts/analysis_cfg.yaml
# 仿真数据分析：
python3 scripts/analysis.py --cfg scripts/simu_analysis_cfg.yaml
```

## 依赖
- Eigen3
- yaml-cpp
- C++17，CMake ≥ 3.10
- matplotlib
- numpy
- scipy