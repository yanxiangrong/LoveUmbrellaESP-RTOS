# LoveUmbrellaESP-RTOS

第二代爱心雨伞控制器程序，基于 FreeRTOS 操作系统。

> 爱心雨伞控制器使用 [ESP8266开发板](https://github.com/HNIT-IoT-Lab/ESP8266-board) 作为核心板。

> 测试中的控制器
![IMG_20220331_123436](https://user-images.githubusercontent.com/39958055/160977423-13001bba-1a12-4f85-81d6-6290b2efd2ae.jpg)

> 装上去的控制器
![IMG_20220406_130902 (自定义) (1)](https://user-images.githubusercontent.com/39958055/161901480-f2683a43-afcc-4a61-a926-96400d476b02.jpg)


该核心板以KCP协议与服务器通信，接收服务器下发指令，来控制锁机、广告机、氛围灯的开和关，向服务器报告用户借/还伞是否完成。

## Todo

- [x] 在不重刷固件的情况下更新 WIFI 配置
  - [x] 使用微信小程序配网 (AirKiss 协议)
  - [ ] ~~使用 APP 配网~~
  - [x] 重新配网 (复位5次恢复出厂设置)
- [x] 与服务器通信
  - [x] 嵌入 KCP 协议
  - [x] 连接服务器
  - [ ] 加密通信
- [x] 使用 Json 格式通信
  - [x] Json 格式约定
  - [x] Json 解析
- [x] 时钟功能
  - [x] 使用DS1307模块获取时间
  - [x] 联网自动校准时间 
- [x] 使用 LED 灯展示状态
- [ ] 使用 蜂鸣器 展示状态
- [x] 用传感器感知用户是否取伞完成
