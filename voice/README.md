# 语音端接口｜BOX0 开发板 ↔ Xiaozhi Server

此处是**语音 BOX0 开发板**的接口，**不是传感器 ESP32 开发板**。BOX0 固件基于 [xiaozhi-esp32](https://github.com/78/xiaozhi-esp32)。软件端使用 [xiaozhi-esp32-server](https://github.com/xinnan-tech/xiaozhi-esp32-server) 承接 BOX0 的音频连接，使用 [DoorMinder Provider](https://github.com/Joycealien/doorminder/tree/feature/box0-xiaozhi-bridge/voice_bridge) 对接业务后端。

| 硬件侧提供/负责 | 软件端提供/负责 |
| --- | --- |
| BOX0 固件、联网、麦克风与扬声器；将设备的 OTA/配置地址指向联调服务 | 可从 BOX0 所在网络访问的 OTA/配置接口和 WebSocket 服务 |
| 按固件原有 Xiaozhi 协议连接、上传语音、播放回复 | 部署服务并配置可用的 ASR、TTS；DoorMinder Provider 把识别文字送入业务后端 |
| 用服务端给出的地址与协议版本完成联调 | 向硬件侧提供实际 OTA URL、WebSocket URL、协议版本以及必要的认证配置 |

业务后端接口由软件端维护。本接口文档约定 BOX0 与语音服务端的连接边界及当前联调地址。

## 两个地址

1. **OTA/配置 URL**：BOX0 上电后访问的 HTTP 接口。它向设备下发 WebSocket 的 `url`、可选 `token` 和 `version` 等配置；它也可用于固件升级检查。
2. **WebSocket URL**：BOX0 开始语音会话时连接的语音服务。文本帧是 JSON 控制消息，二进制帧是 Opus 音频。

当前本机联调配置如下。`192.168.110.25` 是本次电脑的 WLAN 地址；实物接入时按 BOX0 所在网络重新确认可达 IPv4：

```text
OTA/配置：http://192.168.110.25:8003/xiaozhi/ota/
WebSocket：ws://192.168.110.25:8001/xiaozhi/v1/
```

OTA 响应中的 WebSocket 配置示意：

```json
{
  "websocket": {
    "url": "ws://192.168.110.25:8001/xiaozhi/v1/",
    "version": 1
  }
}
```

以上仅展示与语音连接相关的字段；完整 OTA 响应以实际服务实现为准。当前本地 xiaozhi-esp32 v2.0.3 源码优先读取设备保存的 `wifi.ota_url`，没有时使用编译配置 `CONFIG_OTA_URL`；从 OTA 响应的 `websocket` 对象保存连接配置。服务端的简单部署与全模块部署可能采用不同 OTA 端口，见[服务端固件对接说明](https://github.com/xinnan-tech/xiaozhi-esp32-server/blob/main/docs/firmware-build.md)。

## WebSocket 协议要点

建议第一轮统一 `Protocol-Version: 1`。当前所用 v2.0.3 固件在握手时带 `Protocol-Version`、`Device-Id`、`Client-Id`，配置了 token 时还带 `Authorization: Bearer <token>`。这些设备标识由固件生成；服务端认证策略由软件端约定。

连接建立后，设备先发送 `hello`。以下音频参数来自当前固件，不应把它们当作服务端下行参数：

```json
{
  "type": "hello",
  "version": 1,
  "features": {"mcp": true},
  "transport": "websocket",
  "audio_params": {
    "format": "opus",
    "sample_rate": 16000,
    "channels": 1,
    "frame_duration": 60
  }
}
```

服务端需返回 `type: "hello"`、`transport: "websocket"`；可返回 `session_id` 与下行 `audio_params`。设备当前等待服务端 `hello` 的超时为 10 秒。版本 1 的 WebSocket 二进制帧直接承载 Opus 数据，不附加版本 2/3 的二进制头。

| 方向 | 常见消息 | 用途 |
| --- | --- | --- |
| BOX0 → Server | `listen/start`、二进制 Opus、`listen/stop` | 一次语音输入；部分自动模式由服务端判断结束 |
| BOX0 → Server | `abort` | 中断当前回复 |
| Server → BOX0 | `stt` | 识别文字，可显示在设备上 |
| Server → BOX0 | `tts/start`、`tts/sentence_start`、二进制 Opus、`tts/stop` | 回复文字与语音播放 |

`listen` 消息还包含 `mode`（如 `auto` / `manual`），具体发送顺序随固件工作模式变化。设备还支持 `mcp` 等消息；首轮只需验证基础对话，后续设备控制再单独约定。完整帧格式见 [xiaozhi-esp32 WebSocket 协议](https://github.com/78/xiaozhi-esp32/blob/main/docs/websocket.md)。

## 与业务后端的关系

```text
BOX0 板 ── WebSocket ──→ Xiaozhi 语音服务端 ── HTTP ──→ DoorMinder 业务后端
传感器 ESP32 板 ── HTTP POST /event ───────────→ 业务后端
```

BOX0 不直接接收传感器板的 `POST /event`。Xiaozhi 的 DoorMinder Provider 将 ASR 文字经 `POST /api/voice/session/open` 和 `POST /api/assistant/input`（`device: "box0"`）送给业务后端，收到 `reply` 后由 TTS 播报。后端保留传感器已触发的 HOME/LEAVE 模式。完整部署步骤见 [语音桥接说明](https://github.com/Joycealien/doorminder/blob/feature/box0-xiaozhi-bridge/voice_bridge/README.md)。

## 联调交付与验收

软件端提供可访问的 OTA URL、该 OTA 响应实际下发的 WebSocket URL、协议版本和认证要求。2026-09-28 已用实际 BOX0 在同一局域网完成 OTA、WebSocket/`hello`、Opus 上传、ASR、DoorMinder 固定测试回复、TTS 下发和设备播报。设备曾将“今天带钥匙”识别并播报固定回复，服务端与后端接口返回正常。设备屏幕在会话后仍会出现“服务器已断开，请唤醒或者按下M键重连”提示，并有断开后重连记录；提示原因尚待确认。该设备外壳标注 BOX0，OTA 上报固件型号 `atk-dnesp32s3-box-v0`、版本 `1.6.6`，与本地准备的 BOX0 `v2.0.3` 固件名称不同；刷机前须核对硬件版本。正式业务模型回复和[传感器事件接口](../sensor/README.md)的实物联动尚未验证。
