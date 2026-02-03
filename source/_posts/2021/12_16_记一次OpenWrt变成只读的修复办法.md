---
disableNunjucks: true
mathjax: false
title: 记一次OpenWrt变成只读的修复办法
tags:
  - Linux
  - OpenWrt
categories:
  - 瞎折腾
description: 这篇文章介绍了一次意外断电导致OpenWrt变成只读系统的问题，分析确认是断电引发而非空间不足，最后通过e2fsck指令修复并重启解决了问题。
date: 2021-12-16 18:10:43
---

> 一次意外断电，导致OpenWrt变成了只读系统。
使用`e2fsck`指令可以自动修复，最后需重启。

<!-- more -->

```sh
# root @ OpenWrt in ~ [18:12:12]
$ df -h
Filesystem                Size      Used Available Use% Mounted on
/dev/root                57.5G     21.6G     36.0G  37% /
tmpfs                     1.9G     18.2M      1.8G   1% /tmp
/dev/mmcblk0p1           63.9M     16.4M     47.4M  26% /boot
tmpfs                   512.0K         0    512.0K   0% /dev
/dev/root                57.5G     21.6G     36.0G  37% /opt/docker
```

查看`df -h`输出的信息，显然`/dev/root`远还没有满，排除空间不足的原因。
后来想到似乎有一次意外断电，在操作插排时拔错了线。

修复方案：

```sh
e2fsck /dev/mmcblk0p2
```

然后一路点`y`就好了。

重启。

[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
