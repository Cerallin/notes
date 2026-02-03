---
disableNunjucks: false
mathjax: false
title: 如何在树莓派4B上快速重装home-assistant
tags:
  - home-assistant
  - 树莓派
  - Linux
categories:
  - 瞎折腾
date: 2025-05-04 08:56:15
---

> home-assistant（HA）是一个Python编写的开源智能家居中枢系统，可以运行在各种各样的硬件上，甚至包括树莓派和软路由，官方为边缘计算设备还推出了打包好HA的操作系统，HA OS。然而，最近的HAOS在树莓派4B上会遇到bug，无法启动。每当更新系统失败的时候就需要重装，回滚到正常的版本。本文作为备忘录记录树莓派安装HAOS的流程和常见问题。

<!-- more -->

### 准备工作

#### 硬件准备

安装教程可以参考[HA官方网站](https://www.home-assistant.io/installation/raspberrypi/)

- 树莓派4B及以上（3B+及以下没有足够内存）
- SD卡读卡器（用来连接电脑烧录系统进去）
- 网线（HA OS初始化的时候必须先用网线连接，之后可以无线连接）

#### 网络环境

网络环境需要满足以下几点要求：

- 能访问外网
- 能访问docker.io

在安装之前一定要配置好透明代理。HA OS初始化的时候并不能设置代理，至少我没有找到方法。

### 安装步骤

#### 烧录系统（5 min）

首先去[HA OS Github Release 页](https://github.com/home-assistant/operating-system/releases)下载合适版本的镜像，比如haos_rpi4-64-15.0.img.xz。
名字里的数字依次表示树莓派4、64位操作系统、15.0版本。

<details>
<summary>[如果想安装最新的HA OS系统，可以不从Github下载。]{}</summary>

本教程写作的现在，HA OS最新的两个版本15.2和15.1，都会导致树莓派4黑屏，恶性bug。
因此，不得不先手动下载无害的15.0版本。

</details>

然后使用树莓派官方烧录器，如[@fig:rpi-imager]所示，从左到右依次选择：

- 设备：Raspberry Pi 4；
- 操作系统：拉到最下面选“Use Custom”，然后从文件管理器里选择刚才下载的系统镜像；
- SD卡：可不要选到硬盘去了。

![树莓派官方镜像烧录器，Raspberry Pi Imager](/notes/images/raspberry-imager.png){.fig-ref #fig:rpi-imager }

全部选择完毕后点击Next，如果弹窗问是否保存默认用户配置，就“下次一定”，因为HA OS没有机会设置自定义的用户名之类的。

如果想安装最新版的HA OS，在烧录器页面选操作系统的时候，可以选“Other specific-purpose OS -> Home assistants and home automation -> Home Assistant”。

#### 初始化与备份恢复（120 min）

插入SD卡、网线，通电。等待最多5分钟，就可以访问HA的网页了。
等待期间可以上路由器找一下树莓派的IP地址，或者确认一下内网域名是lan还是local。
通过`homeassistant.local:8123`或者`IP地址:8123`访问HA网页，可以看到初始化进度。
此时还需等待至少20分钟……

初始化成功后，选择从备份恢复。
没备份就跳过此步。
上传相应的备份，有密码的话还要输入密码。
从备份恢复往往需要若干小时……

#### 启用I2C（10 min）

终于进入HA系统了！
熟悉得就像在家里一样。（本来就在家里吧）

为了监控电池电量，我安装了一个[基于I2C的插件](https://github.com/odya/hass-ina219-ups-hat)。
但是，每次重装之后都需要先重新启用I2C功能。

我使用[这篇帖子](https://community.home-assistant.io/t/add-on-hassos-i2c-configurator/264167)里介绍的[这个加载项](https://github.com/adamoutler/HassOSConfigurator)修改启用I2C，步骤如下：

{% link_card "点我一键跳转" "https://my.home-assistant.io/redirect/supervisor_add_addon_repository/?repository_url=https%3A%2F%2Fgithub.com%2Fadamoutler%2FHassOSConfigurator" %}

1. 点击上方神秘链接添加加载项仓库到HA。注意下方可以选择HA网页地址。
2. 在HA里选择“设置 -> 加载项 -> 搜索I2C -> HassOS I2C Configurator -> 安装”。
3. 静静地等待转圈圈，此时不能乱动否则需要从头安装。迟迟安装不上请检查网络环境。
4. 关闭该加载项的“保护模式”，然后启动。
5. 循环2-3次：硬重启->启动该加载项->硬重启->启动该加载项。

据加载项作者说，一共需要重启两次，一次是为了让配置落盘，一次是让HA以更新了的配置启动。
但我每次都要重启三次才好使。

### 总结

本文介绍了树莓派4B上home-assistant的快速重装方法。
方法是通用的，树莓派5也可以用。
观察HA OS的Github页面可知，各种树莓派上时不时就会有无法启动/启动慢的问题。

这个故事告诉我们HA OS升级需谨慎。

### 附录：树莓派无法启动的表现 {data-toc-unnumbered=true}

树莓派上电之后红灯常亮，绿灯闪烁。
10分钟之后绿灯常亮，偶尔闪烁。
如果接屏幕启动，屏幕一直黑屏。
<!-- Finish -->
[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
