# ESP32 Home Event Sensor

## 项目说明

ESP32 负责通过 WiFi 向后端上报家庭状态事件。

当前只上报两种事件：

- `HOME_EVENT`：用户回到家
- `LEAVE_EVENT`：用户离开家

设备端只负责采集、判断和传输事件，不负责用户状态管理、提醒策略或数据库逻辑。

## 后端接入接口

ESP32 向后端发送：

```http
POST /event
Content-Type: application/json
```

请求体：

```json
{
	"event": "HOME_EVENT",
	"device": "esp32_sensor"
}
```

`event` 的值为 `HOME_EVENT` 或 `LEAVE_EVENT`，`device` 当前固定为 `esp32_sensor`。

后端收到事件后，直接根据 `event` 处理业务，不需要依赖 ESP32 使用了什么传感器或按键。

建议成功响应：

```json
{
	"status": "ok"
}
```

## 本地联调

仓库中的 `server.py` 是一个简单的 Flask 接收示例：

```bash
python server.py
```

默认监听 `0.0.0.0:5000`，接口地址为：

```text
http://<后端设备IP>:5000/event
```

ESP32 端需要将 `esp32_sensor.ino` 中的 `serverUrl` 修改为实际后端地址，并配置 WiFi 信息。

## 文件说明

| 文件 | 说明 |
| --- | --- |
| `esp32_sensor.ino` | ESP32 端程序，负责上报事件 |
| `server.py` | Flask 本地联调服务 |
| `event_api.md` | 接口细节说明 |