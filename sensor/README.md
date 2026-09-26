# 传感器端接口｜ESP32 开发板

此处是**传感器开发板**的接口，**不是 BOX0**。传感器 ESP32 识别回家/离家事件，通过 WiFi 直接上报业务后端。后端依据事件类型处理业务，无需依赖具体按键或传感器实现。传感器板不管理用户状态、提醒策略、Agent 或数据库。

## 事件上报

```http
POST /event
Content-Type: application/json
```

请求体：

```json
{
  "event": "LEAVE_EVENT",
  "device": "esp32_sensor"
}
```

| 字段 | 类型 | 当前值 | 含义 |
| --- | --- | --- | --- |
| `event` | string | `LEAVE_EVENT` / `HOME_EVENT` | 离家 / 回家事件 |
| `device` | string | `esp32_sensor` | 设备标识，当前示例固定值 |

当前固件只发送以上两个字段；**时间由接收端记录**。建议后端成功接收后返回 HTTP 2xx；联调示例返回：

```json
{"status":"ok"}
```

示例固件没有发送队列和失败重试，业务端若需去重、持久化或可靠投递，应另行约定。

## 本地联调

1. 安装 Flask：`python -m pip install flask`。
2. 在本目录运行：`python server.py`。示例服务监听 `0.0.0.0:5000`，接收 `POST /event`，并提供 `GET /status` 和 `GET /` 供查看最近一次事件；重启后记录清空。
3. 在 [`esp32_sensor.ino`](esp32_sensor.ino) 中填写测试 WiFi 和 `serverUrl`，例如 `http://<后端局域网IP>:5000/event`。ESP32 与测试机需能互通。
4. 当前演示程序用 GPIO18 按键触发 `LEAVE_EVENT`，GPIO19 触发 `HOME_EVENT`；GPIO23、GPIO25 的低电平 LED 分别提示对应按键事件。按键只是测试输入，正式传感器可替换，HTTP 报文保持一致。

业务后端只需按本页 `POST /event` 的请求格式接入。`GET /status`、`GET /`、GPIO 接线和 Flask 实现均属于本地演示，不是必须实现的业务接口。BOX0 的语音链路另见[语音端接口](../voice/README.md)。
