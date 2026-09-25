# ESP32 Sensor Node Event API

## 1. Overview

本模块为家庭提醒助手的传感器端节点，负责采集设备状态并产生事件。

当前版本使用 ESP32 开发板，通过 WiFi 网络将事件信息发送至后端服务。

设备端职责：

- 传感器数据采集
- 事件判断
- 事件生成
- 事件上传

设备端不负责：

- 用户状态管理
- 提醒策略判断
- AI Agent 逻辑
- 数据库存储


整体流程：

```text
Sensor Node
        |
    Event
        |
 HTTP POST
        |
Backend Service
        |
AI Agent / Database / App
```


---

## 2. Communication

## Protocol

```http
HTTP POST
```


## Content-Type

```json
application/json
```


## Endpoint

```http
POST /event
```


---

## 3. Event Message Format

## Request Body

Example:

```json
{
    "event": "LEAVE_EVENT",
    "device": "esp32_sensor"
}
````

## Field Description

| 字段   | 类型   | 说明     |
| ------ | ------ | -------- |
| event  | string | 事件类型 |
| device | string | 设备编号 |

---

## 4. Supported Events

## 4.1 LEAVE_EVENT

含义：

用户离开家庭环境。

Example:

```json
{
    "event": "LEAVE_EVENT",
    "device": "esp32_sensor"
}
```

---

## 4.2 HOME_EVENT

含义：

用户返回家庭环境。

Example:

```json
{
    "event": "HOME_EVENT",
    "device": "esp32_sensor"
}
```

---

## 5. Future Events

后续根据项目需求扩展：

| 事件         | 说明       |
| ------------ | ---------- |
| KEY_TAKEN    | 钥匙被取走 |
| REMIND_DONE  | 提醒完成   |
| PLAN_UPDATED | 用户计划更新 |

Example:

```json
{
    "event": "KEY_TAKEN",
    "device": "key_sensor"
}
```

---

## 6. Backend Requirements

后端收到事件后，不应该依赖设备内部实现。

例如：

错误：

```
GPIO18触发
=
用户出门
```

正确：

```
收到 LEAVE_EVENT
=
处理离开事件
```

原因：

设备端检测方式可能变化：

当前：

```
按键模拟
    |
LEAVE_EVENT
```

未来：

```
门磁/人体检测/AI判断
    |
LEAVE_EVENT
```

后端接口保持不变。

---

## 7. Test Server

当前提供 Flask Demo：

目录：

```
server_demo/
    server.py
```

功能：

## Receive Event

```
POST /event
```

## Query Current Status

```
GET /status
```

## Web Display

```
GET /
```

该服务器仅用于：

- ESP32 通信测试
- 接口验证
- 开发阶段调试

正式环境可替换为：

- Node.js
- FastAPI
- Spring Boot
- 云服务器接口

---

# 8. ESP32 Hardware Demo

当前测试版本：

## Input

按键模拟事件：

| 功能   | GPIO   |
| ---- | ------ |
| 出门事件 | GPIO18 |
| 进门事件 | GPIO19 |

## Output

LED:

| 功能   | GPIO   |
| ---- | ------ |
| 出门状态 | GPIO23 |
| 进门状态 | GPIO25 |

LED:

```
LOW -> ON
HIGH -> OFF
```

---

# 9. Integration Example

Backend receives:

```json
{
    "event":"LEAVE_EVENT",
    "device":"esp32_sensor"
}
```

Recommended processing:

```
LEAVE_EVENT

      |

Update user status

      |

Check pending plans

      |

Trigger reminder if needed
```

---

# 10. Notes

当前 ESP32 端只提供事件能力。

后续新增传感器：

* 门磁
* PIR
* 超声波
* 钥匙检测

只需要保持事件格式一致，无需修改后端逻辑。

