---
disableNunjucks: true
mathjax: true
title: 使用logrotate自动分割nginx日志
tags:
  - Nginx
  - Linux
  - 运维
  - 压缩包
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了真白萌论坛由于庞大的日志量需要通过logrotate工具管理日志文件，实现按天分割、保留14天日志以及节约硬盘空间的问题。文章描述了logrotate工具的功能、运行机制和配置路径，强调其在大多数Linux发行版中的默认支持。最后提供了一个实例配置文件并说明如何启用和强制运行logrotate。
date: 2022-01-01 13:32:33
---

>某真〇萌论坛的日志量及其庞大，从8月到现在的Nginx日志竟有44G。
查阅资料，发现大多数Linux发行版默认使用logrotate对各种日志进行分割，且能满足我的需求。

<!-- more -->

### 需求分析

真〇萌论坛的日志量很大，为了节约服务器硬盘空间，有必要定期删除陈旧的日志文件。
核心需求有两个：

1. 保留14天以内的日志。
2. 将日志按天分割到不同的文件里。

### 一种可行的解决方案

查阅资料，发现大多数Linux发行版默认使用logrotate对各种日志进行分割，且能满足我的需求。

>logrotate是一个linux系统日志的管理工具。
可以对单个日志文件或者某个目录下的文件按时间/大小进行切割，压缩操作；指定日志保存数量；还可以在切割之后运行自定义命令。
logrotate是基于crontab运行的，所以这个时间点是由crontab控制的，具体可以查询crontab的配置文件/etc/anacrontab。
系统会按照计划的频率运行logrotate，通常是每天。
在大多数的Linux发行版本上，计划每天运行的脚本位于/etc/cron.daily/logrotate。
主流Linux发行版上都默认安装有logrotate包，如果你的linux系统中找不到logrotate，可以使用apt-get或yum命令来安装。

设置logrotate的配置文件`/etc/logrotate.d/nginx`内容如下：
```conf
/www/wwwlogs/www.masiro.me.log {
    daily
    rotate 14
    missingok
    notifempty
    compress
    nodelaycompress
    copytruncate
    dateext
    dateformat -%Y-%m-%d
    dateyesterday
}
```
其中各项的含义如[@tbl:logrotate-explanation]所示。

项                    含义
-------               ----------------------------------
daily                 按天切割
rotate                保留14个日志文件
missingok             切割中遇到日志错误忽略
notifempty            日志如果为空将不进行切割和压缩
compress              以gzip方式压缩
nodelaycompress       不要将刚切割后的日志文件放到下个循环中进行压缩
copytruncate          复制源日志文件后并清空源日志文件
dateext               切割后的文件添加日志扩展名
dateformat            扩展方式为 -%Y-%m-%d
dateyesterday         切割后的文件日志命名为昨天的日期

: logrotate选项含义表 {#tbl:logrotate-explanation}

### 启用logrotate的自动任务

```sh
logrotate /etc/logrotate.d/nginx
```

### 强制当场运行logrotate

```sh
logrotate -vf /etc/logrotate.d/nginx
```

[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
