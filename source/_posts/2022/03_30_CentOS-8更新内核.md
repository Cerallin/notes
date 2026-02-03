---
disableNunjucks: true
mathjax: false
title: CentOS 8更新内核
tags:
  - Linux
  - CentOS
  - 运维
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了Linux内核被爆出漏洞后在CentOS
  8中的更新方法，包括配置更新源、安装ELREPO源和内核升级的具体步骤，同时提供了国内常用镜像源选择。
date: 2022-03-30 10:36:32
---

> Linux内核被爆出漏洞，需要更新。

<!-- more -->

### 实验环境

系统：CentOS 8
内核版本：4.18.0-240.1.1.el8_3.x86_64

### 实验步骤

#### 更新源

2022年1月31日，CentOS团队终于从官方镜像中移除CentOS 8的所有包。
现在他们被转移到[https://vault.centos.org](https://vault.centos.org)。

编辑`/etc/yum.repos.d/`目录下的文件：

```
CentOS-Linux-BaseOS.repo
CentOS-Linux-AppStream.repo
CentOS-Linux-PowerTools.repo
```

```sh
# [baseos]
baseurl=https://vault.centos.org/centos/$releasever/BaseOS/$basearch/os/
# [appstream]
baseurl=https://vault.centos.org/centos/$releasever/AppStream/$basearch/os/
# [powertools]
baseurl=https://vault.centos.org/centos/$releasever/PowerTools/$basearch/os/
```

#### 安装elrepo源

```sh
rpm --import https://www.elrepo.org/RPM-GPG-KEY-elrepo.org
yum install https://www.elrepo.org/elrepo-release-8.el8.elrepo.noarch.rpm
```

当然，国内用户可以自行选择清华、中科大或者阿里源。

#### 安装内核

```sh
# 查看可安装版本
yum --disablerepo="*" --enablerepo="elrepo-kernel" list available
# kernel-lt是目前的lts版本，5.4.188
yum --enablerepo="elrepo-kernel" install kernel-lt
```

#### 更新引导

```sh
grub2-set-default 0
```

#### 验证内核版本

```sh
$ uname -r
5.4.188-1.el8.elrepo.x86_64
```

[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
