---
disableNunjucks: true
mathjax: false
title: 如何从零配置PixelMon服务器
tags:
  - Java
  - Linux
  - MineCraft
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了在一台12核64G的服务器上部署抛壳萌游戏服的全过程，包括使用Java 8、安装MC Forge Server、以及设置systemd
  unit和其他必要配置的步骤。文中详细说明了在Oracle停止Java
  8免费支持时，从GitHub仓库获取所需版本的解决方法，以及安装Pixelmon服务器和客户端所需的注意事项。提供了相关工具下载链接和配置建议，全程确保游戏服与客户端版本匹配为1.12.2。
date: 2021-12-21 19:55:01
---

> 在一台12核64G服务器上部署一个抛壳萌游戏服。

<!-- more -->

### 实验环境

<div class="mt-6 mb-4 overflow-x-auto">

| 项     | 值                                             |
| :----- | :--------------------------------------------- |
| CPU    | Intel(R) Xeon(R) CPU E5-2603 v3 @ 1.60GHz * 12 |
| Memory | 64G                                            |
| System | Linux 5.4.0-75-generic Ubuntu 20.04 Server     |
: 服务器配置表

</div>

### 准备工作

#### Java8

根据[PixelMon Wiki](https://pixelmonmod.com/wiki/Server_installation#Requirements)的介绍，Java版本必须是64位Java 8，版本稍微变动都不行。

在Ubuntu系统上安装Java 8，几年前倒是折腾过不少次。
~~当时是先安装ppa源，然后更新apt，最后下载，可能需要更改一下默认的Java版本，就结束了。~~

然而Oracle停止维护了Java 8，这种方法，它失效了。
现在需要去Oracle官网下载Java 8，但是Oracle开始收费了，还需要登录才能下载。
于是我们选择[这个GitHub仓库](https://github.com/frekele/oracle-java)下载。
以`jdk-8u212`为例，

```sh
sudo mkdir /usr/lib/java
sudo tar -zxvf jdk-8u212-linux-x64.tar.gz -C /usr/lib/java
sudo update-alternatives --install /usr/bin/java java /usr/lib/java/jdk1.8.0_212/bin/java 300
sudo update-alternatives --config java
```

最后一行运行时根据提示把当前默认Java版本修改为Java 8。

#### MC forge server

从[Forge官网](https://files.minecraftforge.net/net/minecraftforge/forge/index_1.12.2.html)下载1.12的forge installer jar包，然后运行

```sh
java -jar forge-1.12.2-*-installer.jar --installServer
```

**P.S.** MC版本必须是`1.12.2`，请根据这个版本选择最新的forge版本。

**P.S.** 速度可能会慢，请自行设置代理。`export HTTP_PROXY`这种或许不会有效，请尝试使用`proxychains`等工具。

#### 设置systemd unit与其他配置

参考这个GitHub项目：[MinecraftSystemdUnit](https://github.com/agowa338/MinecraftSystemdUnit)。

#### 安装PixelMon

服务器和本地mod都要从官网下载[官网](https://reforged.gg)。服务器版是下方小字的`wget`：

```sh
wget -O Pixelmon-1.12.2-8.3.5.jar https://dl.reforged.gg/3mcvTq2
```

本地mc也要安装mod，且服务器和本地mc版本都是1.12.2。

[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
