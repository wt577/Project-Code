# &nbsp;						复杂环境无人机丢失找寻系统

# \*\*UAV Recovery \& Edge-AI Anomaly Detection System based on Dual-STM32\*\*

# 

# !\[STM32](https://img.shields.io/badge/Hardware-Dual%20STM32-blue.svg) !\[RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-orange.svg) !\[AI](https://img.shields.io/badge/Edge%20AI-NanoEdge%20AI-green.svg) !\[Protocol](https://img.shields.io/badge/Protocol-MAVLink%201.0-lightgrey.svg) !\[Network](https://img.shields.io/badge/Network-4G%20LTE-red.svg)

# 

# 本项目针对无人机在复杂环境（如山区、森林、强磁干扰区）下易发生偏航、坠落及失联等痛点，研发了一套\*\*基于端侧AI（Edge AI）和云边协同的无人机监测与找寻系统\*\*。系统创新性地采用双 STM32 主控架构，结合 FreeRTOS 实时操作系统与 NanoEdge AI 机器学习算法，实现无人机飞行姿态的实时诊断与丢失后的精准云端定位追踪。

# 

# ---

# 

# \## 核心特性 (Core Features)

# 

# \- \*\*边缘 AI 异常诊断\*\*：脱离庞大的上位机算力，在 STM32 裸机端部署 \*\*NanoEdge AI\*\* 机器学习模型。通过采集 MPU6050 与 MS5611 等传感器数据，实现对“坠落、严重偏航、剧烈震荡”等飞行异常状态的毫秒级感知与规避。

# \- \*\*双核异构与高并发调度\*\*：创新设计双 STM32 主控架构，彻底解决单核 MCU 处理高频传感器融合与 4G 通信的算力瓶颈。基于 \*\*FreeRTOS\*\* 实现 8 路高优先级任务无冲突并发调度，系统整体响应效率提升 \*\*30%\*\*。

# \- \*\* 协议栈深度优化\*\*：重构并优化了 \*\*MAVLink 1.0\*\* 协议的数据交互逻辑，打通了 USART 与飞控底层的数据链路，有效解决了极端环境下高频串口通信的丢包问题。

# \- \*\* 4G 全天候云端追踪\*\*：搭建 4G 无线透传链路，无缝接入\*\*原子云（Atom Cloud）\*\*物联网平台。一旦检测到异常坠落，系统将强制锁定最后有效 GPS 坐标并实时上报，保障复杂地形下的快速搜救。

# 

# ---

# 

# \## 系统架构设计 (System Architecture)

# 

# 系统分为\*\*“飞控感知核心”\*\*与\*\*“云端通信核心”\*\*两大模块，通过高速 SPI/USART 进行核间通信：

# 

# ```text

# \[无人机飞控系统] <--- (MAVLink 1.0) ---> \[STM32 Core 1: 数据与AI感知]

# &nbsp;                                          |-- MPU6050 (六轴IMU) -> I2C

# &nbsp;                                          |-- MS5611 (气压计)  -> SPI

# &nbsp;                                          |-- NanoEdge AI 异常诊断引擎

# &nbsp;                                          |

# &nbsp;                                    (核间通信协同)

# &nbsp;                                          |

# &nbsp;                                     \[STM32 Core 2: 任务与通信网关]

# &nbsp;                                          |-- FreeRTOS (8路并发任务调度)

# &nbsp;                                          |-- 异常日志本地存储 (EEPROM)

# &nbsp;                                          |-- 4G LTE 模块 (无线透传)

# &nbsp;                                                 |

# &nbsp;                                                 v

# &nbsp;                                     \[原子云平台 \& PC/APP 控制端]



