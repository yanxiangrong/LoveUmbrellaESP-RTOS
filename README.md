# 爱心雨伞控制器-RTOS

第二代爱心雨伞控制器程序，基于 FreeRTOS 操作系统。

> 爱心雨伞控制器使用 [ESP8266开发板](https://github.com/HNIT-IoT-Lab/ESP8266-board) 作为核心板。

## 项目简介

> 该项目灵感来自于往届学长带领的团队开发的第一代共享雨伞。

该项目是致力于打造一款用于校园环境的共享雨伞，用户可以使用微信扫码用伞（类似于共享单车）。方便师生上下课。

在学校内，我们计划在各教学楼和办公楼放置共享雨伞伞架。可能会有人提问，我们是否会在学生宿舍放置共享雨伞伞架？事实上我们无需这么做，因为我们的共享雨伞不按时长收费（目前来说应该是完全免费使用的），用户可以将雨伞带回宿舍，次日再还。

## 技术简介

我们设计并制作了一款雨伞伞架，如下图所示，伞架内部装有一条导轨。雨伞挂于导轨上，导轨末端装有电子锁和传感器，用来控制雨伞进出。

<p align="center">
   <img alt="IMG_20220423_112604 (自定义)" src="https://user-images.githubusercontent.com/39958055/164873555-69317a19-1a1c-4563-a8d2-916dd3532f62.jpg" height="400">
</p>

客户端基于微信小程序设计，用户需要在微信小程序注册，登录，才能借伞。[微信小程序项目地址](https://github.com/HNIT-IoT-Lab/love-umbrella-miniprogram)。  
服务端使用 Java 语言开发，基于 SpringBoot 框架，带有后台管理页面。[服务端使用项目地址](https://github.com/HNIT-IoT-Lab/umbrella-server)。

伞架装有基于ESP8266模块的控制器。


> 测试中的控制器

![IMG_20220331_123436](https://user-images.githubusercontent.com/39958055/160977423-13001bba-1a12-4f85-81d6-6290b2efd2ae.jpg)

> 装上去的控制器

![IMG_20220406_130902 (自定义) (1)](https://user-images.githubusercontent.com/39958055/161901480-f2683a43-afcc-4a61-a926-96400d476b02.jpg)


该核心板以KCP协议与服务器通信，接收服务器下发指令，来控制锁机、广告机、氛围灯的开和关，向服务器报告用户借/还伞是否完成。

## 软件简介

> 开发工具: Clion + PlatformIO  
> 开发环境: Windows 11 + PlatformIO_NodeMCU_1.0-esp8266-rots-sdk + Clion 捆绑的MinGW(版本w64 9.0)

控制器软件我们起初基于 Arduino-core 开发，后改用基于 RTOS 实时操作系统开发。

我们使用 AirKiss 协议 和 ESPTOUCH 协议来配置 WiFi，ESPTOUCH 协议需要在手机上安装乐鑫提供的 EspTouch APP 来配置，但是经我们测试，始终无法成功配网，具体原因不明。  
在 AirKiss 协议方面，可以使用微信小程序进行配网（在微信小程序搜索 “AirKiss” 关键字即可找到一些配网小程序）。  
配网成功后，WiFi 配置会被保存在 Flash 下次启动时可自动链接。

另外，由于 ESP8266 模块 GPIO 资源较少，我们使用了 PCF8575 GPIO 扩展模块，该模块使用 I2C 协议与 ESP8266 通信。  
时钟部分，时钟模块使用的是 DS1307 模块，时钟的用处非常多，如用于生成随机数，防重放攻击等等。  
为了获得准确的时间，软件中包含有使用 NTP 协议获取时间的部分程序，默认使用阿里云 NTP 服务器。  
在发送 NTP 请求前，程序会先通过 DNS 查询 IP 地址，DNS 服务器默认使用 DHCP 提供的地址。  
随机数生成算法我们采用[梅森旋转算法](https://github.com/dajobe/libmtwist)来生成较高质量的随机数。  
JSON 部分，我们使用的是 SDK 中的 CJSON 库来处 JSON 数据。  


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

## 更多图片和视频

图片和视频储存在我的[阿里云文件站](https://yandage.top/storage/folder/Pictures/爱心雨伞开发记录/)。
