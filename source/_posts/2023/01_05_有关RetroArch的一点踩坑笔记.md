---
disableNunjucks: true
mathjax: true
title: 有关RetroArch的一点踩坑笔记
tags:
  - RetroArch
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了RetroArch作为跨平台模拟器前端的功能及其在Steam版中的配置特点，涵盖了游戏ROM与光盘文件的准备、模拟器核心和系统文件的获取过程，同时提到了为优化游戏画面效果可选择的着色器设置。文章详细记录了Steam版与原版RetroArch的配置相似点与特有步骤，并提供了解决核心识别问题的办法。附加了相关资源链接与技术说明，便于玩家操作与参考。
date: 2023-01-05 14:03:32
---

> RetroArch是一个跨平台的前端软件，为游戏机模拟器、游戏引擎和多媒体播放器提供统一的接口。
自从RetroArch登录了Steam，就可以统计古早游戏的游戏时长了，颇有成就感。
但是，Steam版的RetroArch与原版有些出入，导致有些问题很难通过网络搜索解决。
更何况原版RetroArch配置模拟器后端就有很多坑，Steam版RetroArch的配置难度只增不减。
本文记录了一些Steam版RetroArch的配置流程，绝大多数情况都与原版RetroArch的配置流程相同，还有一部分是Steam版特有的步骤。

<!-- more -->

### 各种文档

- [RetroArch官方网站](https://www.retroarch.com/)
- [RetroArch build bot](https://buildbot.libretro.com/)
- [Libretro文档](https://docs.libretro.com/)

### 准备游戏

下载游戏。

绝大多数游戏文件属于“ROM image”，简称ROM，是只读映像的意思。
不同平台的游戏文件有不同的文件扩展名，例如Game Boy系的游戏可以以`gba/gbc`结尾，NDS的游戏以`nds`结尾。
而PS的游戏，因为是以光盘为载体，一般以`img`或`iso`结尾。

为了成功读取光盘，需要配套的`xx.cue`标记文件，标注了光盘映像的各个区域内容的起止。
对于我们的游戏而言，`CUE`文件结构通常非常简单，因为只需要标注游戏内容的起止范围即可，也就是全部范围。
`CUE`文件内容示例：

```
FILE "/path/to/game.img" BINARY
    TRACK 01 MODE1/2352
    INDEX 01 00:00:00
```

有的大型游戏因为一个光盘装不下，会分成2-3个盘。
虽然读取第一个光盘即可开始游戏，但是RetroArch并没有实现游戏中间换盘的机制。
因此需要在启动游戏之前就告诉模拟器一共有几个游戏光盘。
为此，我们还需要准备一个`m3u`文件，如下所示。
其内容就是按顺序罗列各个光盘的路径，每一行有且仅有一个光盘的路径，文件名里如果有空格也不需要加引号。

```
/path/to/game (A).cue
/path/to/game (B).cue
```

### 准备模拟器

#### 下载模拟器核心

RetroArch是一个游戏机模拟器前端，需要对应的后端才能运行相应的游戏。
libretro团队和社区爱好者们为各主流模拟器实现了Retroarch兼容层，每个打包好的模拟器后端包含3个文件：动态链接库（xxx.dll）格式的模拟器核心、用于RetroArch读取的描述文件和许可证文件~~（出于对开源社区的敬意，但是想删就删吧）~~。

Steam版RetroArch的DLC可以下载一部分支持的模拟器后端，但并不是全部，比如PS2的模拟器PCSX2就没有在Steam上架。
此时我们需要手动从官方的build bot网站下载。
例如，当前RetroArch最新的稳定版版本号为1.14.0，64位Windows系统可以从[这个页面](https://buildbot.libretro.com/stable/1.14.0/windows/x86_64/)点击下载对应的RetroArch_cores.7z。

当然，从Steam下载最大的好处就是可以自动更新模拟器核心，否则需要手动更新（经典模拟器的核心不需要更新这么频繁就是了）。

#### 下载对应的系统文件

虽然模拟器不违法，但是厂商永远有办法给玩家添堵，即使是几十年前就已经停产了的怀旧机型。
无一例外，所有模拟器都依赖于正版游戏机的一部分软件才能完整运行（通常为BIOS）。
出于法律方面的考虑，模拟器开发者不能直接将这部分内容直接提供给玩家，因此需要我们自己去互联网发掘。
模拟器需要什么文件可以查看相关文档确认。
例如，[PCSX2核心推荐使用SCPH-10000型号的BIOS](https://docs.libretro.com/library/pcsx2/#bios)。

### 滤镜与着色器

虽然现在像素风已经成为一种独特的美术风格，但是老游戏运行起来的画面的样子其实并不如设计师所想，也与当年的玩家所见不同。
像素$\neq$糊，谁说当年的游戏就没有在追求立体感和真实感呢？
为了最大地还原游戏想要呈现的显示效果，我们需要合适的着色器（shader）。
例如，为还原psp、nds等隔行扫描的屏幕，可以采用`crt-easymode`的着色器，每两行抽掉一行，得到更接近原版屏幕的显示效果。

当然，正如用钢琴演奏巴洛克时期的键琴作品并不显著地阻碍人们对艺术的欣赏，对游戏画面什么都不做在一部分人看来也许别有一番风味。

萝卜白菜各有所爱，还需自行定夺。

<details>
<summary>
[P.S. Steam版RetroArch不能识别核心的问题。]{}
</summary>

如果发现RetroArch中某个核心不能识别，而显示为`xxx.dll`的情况，则需要额外的步骤来修复。
这是因为RetroArch没有关于该核心的信息。
从[build bot的前端资源](https://buildbot.libretro.com/assets/frontend/)里下载`info.zip`，解压到RetroArch的info文件夹里就可以了。

</details>

[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
