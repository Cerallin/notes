---
disableNunjucks: true
mathjax: false
title: Windows环境下端口转发案例两则
tags:
  - Windows
  - WSL2
  - Linux
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了作者在Windows环境中利用netsh命令实现端口转发的经验，用于解决新台式机与原笔记本电脑之间的服务平衡问题。文章详细分析了WSL2的独立网络特性，并通过两个案例展示了WSL2服务暴露和利用服务器绕过AP隔离的具体实现方法。最后总结了端口转发的配置步骤及其在不同场景中的应用价值。
date: 2024-01-03 14:11:46
---

> 几个月前买了一台新电脑（台式机）放在工位，以前用来办公的笔记本电脑终于可以放在宿舍。
因为办公需要，新电脑必须是Windows系统。
为了写代码，我又安装了WSL2和docker。
在平衡两台电脑的服务的时候，端口转发方面踩了点坑，特此记录下来。

<!-- more -->

### 背景说明

宿舍和工位分别有电脑一台，以下分别称为服务器A和服务器B。
服务器A的ip地址为`10.*.*.*`，其连接的路由器有AP隔离，即A与连接相同路由器的设备之间不能互相访问。
服务器B的ip地址为`222.*.*.*`，网络环境没有AP隔离。
但是服务器B安装了WSL2，WSL2并不与宿主Windows系统公用一套网关，或者说网卡。
也就是说，WSL2有自己的ip地址，并且不是`127.0.0.1`，这与第一代WSL完全不同。

### 技术原理

Windows环境下可以使用`netsh`命令，实现类似于`iptables`的端口转发功能。

使用`netsh interface portproxy show v4tov4`可以查看当前端口转发规则。
新建转发规则的命令如下：

```PowerShell
netsh interface portproxy add v4tov4 \
    listenport=1234 \
    listenaddress=0.0.0.0 \
    connectport=80 \
    connectaddress=192.168.1.1
```
该规则的功能是在1234端口上监听所有地址发来的请求，并转发到`192.168.1.1`的80端口。

上述命令需要在获得了管理员权限的PowerShell/CMD窗口中执行。

### 案例说明

#### 案例一 暴露WSL2的服务

```sh
$ ip addr
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host
       valid_lft forever preferred_lft forever
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq state UP group default qlen 1000
    link/ether 00:15:5d:26:a4:80 brd ff:ff:ff:ff:ff:ff
    inet 172.22.91.163/20 brd 172.22.95.255 scope global eth0
       valid_lft forever preferred_lft forever
    inet6 fe80::215:5dff:fe26:a480/64 scope link
       valid_lft forever preferred_lft forever
```

使用`ip addr`确定WSL2的ip地址为`172.22.91.163`。
然后运行指令，把WSL2的ssh服务暴露到校园网里。

```powershell
netsh interface portproxy add v4tov4 \
    listenport=****** \
    listenaddress=0.0.0.0 \
    connectport=22 \
    connectaddress=172.22.91.163
```

#### 案例二 “往返”绕过AP隔离

上文提到，服务器A所在网络环境有AP隔离，这使得我的ipad有一些看漫画和动画的需求无法实现。
与此同时服务器A和服务器B之间没有AP隔离，经测试是可以互相访问的。
为了绕过AP隔离，我使用服务器B作为跳板，依然在B上添加转发规则，实现“宿舍-工位-宿舍”的网络链路。

运行`netsh interface portproxy show v4tov4`查看配置好的规则。

```
PS C:\WINDOWS\system32> netsh interface portproxy show v4tov4

侦听 ipv4:                 连接到 ipv4:

地址            端口        地址            端口
--------------- ----------  --------------- ----------
0.0.0.0         80          172.22.91.163   80
0.0.0.0         ******      172.22.91.163   22
0.0.0.0         10808       10.**.**.***    10808
```

其中10808是[YACReader](https://www.yacreader.com)服务的端口，一个漫画管理软件。
最后的最后还需要在防火墙里打开相关端口的入站权限，在此不表。

### 结论

本文基于`netsh`命令实现了Windows环境下的端口转发，最终给出两个应用案例，分别实现了WSL2的端口暴露和AP隔离的绕过。

<!-- Finish -->
[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
