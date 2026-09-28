# 硬件接口说明

本项目有**两块独立开发板、两条通信链路**。本仓库原有的 ESP32 程序属于**传感器开发板**；另一块是负责语音交互的**正点原子 BOX0**。队友请按所负责的接口查看对应文档。

| 开发板 | 连接对象 | 对接文档 | 本仓库内容 |
| --- | --- | --- | --- |
| 传感器 ESP32 开发板 | 通过 HTTP `POST /event` 上报给业务后端 | [传感器端接口](sensor/README.md) | [`esp32_sensor.ino`](sensor/esp32_sensor.ino)、本地接收示例 [`server.py`](sensor/server.py) |
| 语音 BOX0 开发板 | 通过 OTA/配置接口取得语音服务地址，再通过 WebSocket 连接 Xiaozhi Server | [语音端接口](voice/README.md) | 对接说明；BOX0 固件不在本仓库 |

```text
传感器 ESP32 板 ── HTTP POST /event ──→ 业务后端
                                           ↑
                                           │ 业务数据交互由软件端约定
                                           │
BOX0 板 ── OTA/配置 + WebSocket ──→ Xiaozhi 语音服务端 :8003/:8001
                                             │
                                             └─ DoorMinder Provider ── HTTP ──→ 业务后端 :8000
```

两块板**不直接通信**。传感器事件进入业务后端；BOX0 的语音经 Xiaozhi Server 识别，由 [DoorMinder 语音桥接层](https://github.com/Joycealien/doorminder/tree/feature/box0-xiaozhi-bridge/voice_bridge) 调用业务后端，再由 Xiaozhi Server 合成语音返回 BOX0。

## 对接边界

- **硬件侧**：维护传感器事件格式、BOX0 固件与服务器地址配置，完成两块板各自的联网和联调。
- **业务后端**：接收传感器事件，管理计划、状态和提醒等业务数据。
- **语音服务端**：基于 [xiaozhi-esp32-server](https://github.com/xinnan-tech/xiaozhi-esp32-server) 部署，提供 BOX0 可访问的 OTA/配置与 WebSocket 服务；通过 DoorMinder Provider 调用业务后端的 `/api/voice/session/open` 与 `/api/assistant/input`。

当前先验证 BOX0 的软件链路与小程序本机调用；实际 BOX0 音频往返和传感器 `HOME_EVENT` / `LEAVE_EVENT` 上报放到实物联调阶段。

> `sensor/server.py` 仅是保存最近事件的本地联调示例，不代表正式业务后端，也不提供语音服务。
