---
disableNunjucks: true
mathjax: false
title: Anaconda重装指南
tags:
  - Python
  - Conda
categories:
  - 计算机与通信工程
description: >-
  这篇文章介绍了作者在复现论文过程中遇到Dscript库安装问题，通过重装Anaconda环境并设置环境变量解决了问题，同时更换了国内源以优化下载速度。文章还提到TensorFlow的安装及其版本变化，并分享了修复cudnn库缺失问题的方法。最后，作者通过下载所需dll文件解决了相关报错并成功运行了程序。
date: 2021-12-16 00:10:29
---

>因为想要复现一篇论文，结果装`Dscript`库都装不上，弄了半天把conda环境搞废了，不如就此重装一下。

<!-- more -->

### 环境变量

默认的Anaconda安装路径为`C:/Users/you/anaconda3`，下面以`HOME`代替。
需要设置三个环境变量：

- `HOME\anaconda3`
- `HOME\anaconda3\Scripts`
- `HOME\anaconda3\Library\bin`

可选的环境变量：

- `HOME\Library\mingw-w64`

我既不做Windows开发，也不做基于mingw的跨平台开发，更不可能用Python开发。
因此，这个选项我还用不上。
有缘人自取之。

设置Windows的环境变量，老规矩，重启生效。
不过接下来的设置没什么需要全局变量的，因此可以在最后再重启。

### Conda初始化

Home键菜单的最近添加里出现了Anaconda Powershell的图标，点开然后执行一句`conda init`，重装之前的东西就又都回来了，比如`xxx.ps1`文件之类的，之前在VSCode里的配置就又重新生效了。

### 换源

国区传统，迄今为止我已经给各种各样的软件管理工具换过源了呢。
使用以下指令添加全套中科大源：

```sh
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/main/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/cloud/conda-forge/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/cloud/msys2/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/cloud/bioconda/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/cloud/menpo/

conda config --set show_channel_urls yes
```

**P.S.** `conda config --set`设置项会被保存到`C:\Users\you\.condarc`文件里。

**P.S.** 据说清华源有问题，咱也不知道咱也不敢问。

### TensorFlow

官网说，自1.15版本起，cpu版和gpu版就合体了，这真是一件好事。
因此只需要一行指令就可以安装了。

```sh
pip install tensorflow
```
### Trouble shooting

>Could not load dynamic library 'cudnn64_8.dll'; dlerror: cudnn64_8.dll not found

仔细想了想我好像是第一次用TensorFlow，之前一直是PyTorch来着。
查阅资料得知，cudnn是针对Deep learning的优化。
但是，cudnn库需要会员身份才能下载。

好在`cudnn64_8.dll`到处都是。
我从[这个网站](https://cn.dll-files.com/)上下载了一份，复制到相关文件夹（比如我放到了将要运行的Python文件的同级目录），就好了。

**P.S.** 本文全篇的you是你的意思，可不是我的名字。

[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
