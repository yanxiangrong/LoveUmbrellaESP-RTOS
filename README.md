# LoveUmbrellaESP-RTOS

第二代爱心雨伞控制器程序，基于 FreeRTOS 操作系统。

> 爱心雨伞控制器使用 [ESP8266开发板](https://github.com/HNIT-IoT-Lab/ESP8266-board) 作为核心板。

## 项目简介

> 该项目灵感来自于往届学长带领的团队开发的第一代共享雨伞。

该项目是致力于打造一款用于校园环境的共享雨伞，用户可以使用微信扫码用伞（类似于共享单车）。方便师生上下课。
在学校内，我们计划在各教学楼和办公楼放置共享雨伞伞架。可能会有人提问，我们是否会在学生宿舍放置共享雨伞伞架？事实上我们无需这么做，因为我们的共享雨伞不按时长收费（目前来说应该是完全免费使用的），用户可以将雨伞带回宿舍，次日再还。

## 技术简介

我们设计并制作了一款雨伞伞架，如下图所示，伞架内部装有一条导轨。雨伞挂于导轨上，导轨末端装有电子锁和传感器，用来控制雨伞进出。

![IMG_20220423_112604 (自定义)](https://user-images.githubusercontent.com/39958055/164873555-69317a19-1a1c-4563-a8d2-916dd3532f62.jpg)

客户端基于微信小程序设计，用户需要在微信小程序注册，登录，才能借伞。[微信小程序项目地址](https://github.com/HNIT-IoT-Lab/love-umbrella-miniprogram)。
服务端使用 Java 语言开发，基于 SpringBoot 框架，带有后台管理页面。
伞架装有基于ESP8266模块的控制器。


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
