---
disableNunjucks: true
mathjax: false
title: 使用BaconFlasher烧录ChisFlash卡带
tags:
  - GBA
  - 烧录
  - 单片机
categories:
  - 瞎折腾
date: 2026-01-20 03:32:46
---

---
bibliography: source/_bibs/gba_chisflash.bib
---


> ChisFlash是开源Game Boy Advanced SP (GBA SP)烧录卡，通过EPM240可编程芯片实现GBA游戏ROM的模拟以及导入导出。
本文介绍通过BaconFlasher (硬件) + FlashGBX (软件) 烧录游戏的流程。

<!-- more -->

<details>
<summary>[本网站不提供未开源的任何固件、软件的下载方式。]{}</summary>

出于尊重原作者的考虑，请访问相应页面加QQ群然后从群文件下载。

</details>

### 前言

Game Boy Advance (GBA) 卡带的存储与烧录机制在硬件结构和软件兼容性层面均具有一定复杂性，尤其体现在存档芯片类型的多样性及其与烧录卡设计之间的耦合关系上。
传统GBA实体卡带主要采用三类存档方案，即FLASH、SRAM与EEPROM，其最大存储容量分别为64 KB（一说32 KB）、128 KB和8 KB[@copetti_gba]。
不同存档方案在电源保持方式、读写时序以及软件调用接口等方面存在显著差异，这也使得通用型GBA烧录卡的设计与使用成为一项具有挑战性的工程问题。

ChisFlash[@chisflash]是由ChisBread（Bread & Moni）提出并开源的一款GBA烧录卡方案。
相较于以EZ-Flash[@ez_flash]为代表的传统商业烧录卡，ChisFlash采用EPM240可编程逻辑器件实现对GBA游戏ROM的模拟及数据导入导出，在保持功能完整性的同时显著降低了硬件成本。

烧录ChisFlash卡带需要NDS或者烧录器。BaconFlasher[@bacon_flasher]是一款硬件层面开源的GBA烧录器，其基本功能是从计算机端读取游戏ROM数据，并按照既定时序将数据写入目标卡带。BaconFlasher与ChisFlash之间可以通过标准卡带插槽或直接针孔连接两种方式进行通信，如[@fig:chisflash-connection]所示。

![硬件连接示意图](/notes/images/graphviz/chisflash硬件连接示意图.svg){ #fig:chisflash-connection}

在软件层面，FlashGBX是目前卡带烧录最成熟的工具之一，可用于卡带信息读取、游戏ROM烧录以及存档数据的导入导出。ChisBread改了一版FlashGBX，用以适配他们的ChisFlash卡带。
然而，烧录游戏的可用性并不完全取决于硬件与软件本身，而更多集中于游戏ROM的存档类型与目标卡带存储结构之间的匹配关系。
针对不同存档类型的游戏，是否需要进行SRAM转换补丁、免电补丁，乃至更换带电池的SRAM卡带，均需要在烧录前进行系统性的判断。

基于上述背景，本文介绍一种通过BaconFlasher和FlashGBX烧录游戏的流程。

### 材料清单

#### 硬件

- [ ] 带USB的电脑1台
- [ ] USB-burner烧录器1个
- [ ] BaconFlash烧录器1个
- [ ] 要烧录的ChisFlash卡带若干

#### 软件

- [ ] Quartus II 13.0 Programmer[@quartusii]：用来配合usb-burner烧录EPM240芯片固件。
- [ ] CH347驱动[@ch347]：用来驱动BaconFlasher。
- [ ] ChisBread改版的FlashGBX[@flashgbx]：读取卡带信息、上传/下载游戏ROM、存档等。

### 烧录前的准备

#### BaconFlasher固件烧录

如[@fig:bacon-flasher-pcb]所示是BaconFlasher的PCB。
BaconFlasher与ChisFlash卡带的连接方式有两种。
一种是通过卡带插槽，也就是PCB下方的大片空白；
与此同时，BaconFlasher还提供了6个针孔，图中叠放ChisFlash卡带PCB，被称为“model 3”。
model 3据说烧录速度很快，但我没有采用。
因为，市面上还没有特地为model 3留出孔位的卡带外壳，也就是说，使用model 3进行烧录，每次都要拧螺丝拆开。
相比之下，通过model 1/2，也就是卡带金手指连接，虽然速度只有150Kb/s，但3-5分钟也烧录完成了。

![BaconFlasher PCB正面图[@bacon_flasher]。红色部分是烧录器本体，下方大片空白是卡带插槽。烧录器左上和右中有两个开关，根据烧录器背面信息调整两个开关，可以改变烧录器读取/写入模式，一共4种：model 0~3。黄色是ChisFlash PCB，可以通过如图所示的叠放连接烧录器右侧的孔位，通过“model 3”高速传输数据。](/notes/images/bacon-flasher.png){.text-justify #fig:bacon-flasher-pcb}

与ChisFlash相同，BaconFlasher也使用EPM240作为控制芯片。
BaconFlasher的嘉立创页面里说：

> 当前为Beta阶段，固件完善后将开源EPM240的全部代码。

截止本文写作时间，还没有开源EPM240的代码，需要进QQ群下载固件`bacon_flasher_beta.pof`。
之后使用Quartus II 13.0 Programmer烧录。

#### 驱动与软件

下载安装CH347厂商驱动[@ch347]，需要注意目前只支持Windows。

安装运行FlashGBX：

```shell
git clone https://github.com/ChisBread/FlashGBX
pip install -v -e .
pip install pyside6 # 为了启用GUI模式
```

#### 游戏ROM预处理

烧录前的最后一项检查：游戏存档类型是否与ChisFlash烧录卡相匹配。

按存档芯片划分，ChisFlash主要有3种卡带：FLASH、SRAM和FRAM。FLASH版卡带主要用于原生FLASH存档类型的游戏，例如Gen 3的宝可梦；SRAM版卡带有带电池和不带电池两种；FRAM芯片价格昂贵，但是断电后存档依然保留。FLASH/EEPROM存档类型的游戏ROM可以通过GBA Tool Advance (GBATA)[@gba_tool_advance,@gbata_romhacking]打补丁从而转换为SRAM格式存档。
如果想用没有纽扣电池的SRAM卡带，还需要再给游戏ROM打免电补丁。
但有些游戏不兼容免电补丁，此时只能换成带电池的SRAM卡带。

总的来说，如果游戏存档类型是FLASH，烧录目标是FLASH卡带，那么不需要给游戏ROM打补丁。
其他情况下，主要是想烧录各种存档类型的游戏ROM到SRAM卡带，需要先打SRAM补丁。
如果SRAM卡带不带电池，还需要再打免电补丁。
如果打了免电补丁的游戏无法保存，则需要改换带电池的SRAM卡带。

![烧录策略图](/notes/images/graphviz/烧录策略.svg){ #fig:savetype-selection}

### 举一个栗子

使用烧录卡有许多好处，不仅可以玩到许多原版游戏，还可以烧录改版。
下面以宝可梦火红的386改版[@firered_386]为例，演示一遍烧录流程。

| Info           | Value                                           |
| :------------- | :---------------------------------------------- |
| Database match | Pokemon - FireRed Version (USA)                 |
| Database       | No-Intro: Game Boy Advance (v. 20210227-023848) |
| File/ROM SHA-1 | 41CB23D8DCCC8EBD7C649CD8FBB58EEACE6E2FDC        |
| File/ROM CRC32 | DD88761C                                        |
: 原版ROM信息[@firered_386] {#tbl:rom-info}

首先访问改版网页。下载链接里提供的是ipa格式的文件。
该文件并不是可以直接拿来玩的游戏ROM，而是定义了如何修改原版ROM的补丁文件。
也就是说，玩家需要自行获取原版ROM，然后通过GBATA将补丁打到ROM上，最终获得一份可玩的改版游戏ROM。

根据网页上的信息我们可以得知，该补丁对应的原版ROM是FireRed Version (USA)，美版火红。

![使用GBATA打开原版美版火红ROM。其中CRC32校验和DD88761C与[@tbl:rom-info]一致。](/notes/images/gbata-firered.png){#fig:gbata-firered}

下载美版火红ROM，首先计算SHA-1校验和，与[@tbl:rom-info]的SHA-1值一致。
[@fig:gbata-firered]显示，该游戏存档类型为1M FLASH，所以我们选取FLASH版本的ChisFlash烧录卡进行烧录。
在烧录之前，还先需要使用GBATA把改版补丁打到ROM上。
选择IPS patcher选项卡，选择补丁文件路径，GBATA会自动生成输出文件路径，如[@fig:gbata-ips-patcher]所示。
此时点击patch，然后生成的文件就可以用于烧录了。

![使用GBATA给ROM打ips补丁。](/notes/images/gbata-ips-patcher.png){#fig:gbata-ips-patcher}

烧录时先把硬件按[@fig:chisflash-connection]连接好，然后启动FlashGBX。

```bash
python -m FlashGBX
```

如[@fig:flashgbx]所示是FlashGBX的GUI页面。
先点击右下角的connect连接到BaconFlasher，如果连接成功左下角会显示烧录器信息。
左侧卡带信息可以手动选择，也可以点击“Analyze Flash Cart”自动获取。
点击“Write ROM”弹出ROM选择窗口，然后就会开始烧录。

![FlashGBX GUI页面。左侧是卡带和ROM信息，右上是操作按钮，右下是烧录进度。先点击右下角connect，连接成功后在左上选择卡带类型：GB/GBA，本文的例子此时应选GBA。](/notes/images/flashgbx.png){.text-justify #fig:flashgbx}

烧录完成后点击右下角disconnect，然后就可以拔下卡带了。

<!-- Finish -->
[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}

### 参考链接 {data-toc-unnumbered=true}
