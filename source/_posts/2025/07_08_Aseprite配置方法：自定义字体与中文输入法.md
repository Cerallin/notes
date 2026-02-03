---
disableNunjucks: true
mathjax: false
title: Aseprite配置方法：自定义字体与中文输入法
tags:
  - C++
  - Windows
  - 像素画
categories:
  - 瞎折腾
date: 2025-07-08 10:41:33
---

> Aseprite是一款老牌（反正比我老）像素画绘图软件，使用原生C++跨平台开发。
该软件虽然开源，但是有独特的EULA许可：可以在个人电脑上编译安装，但不能把编译好的软件分发给任何其他人。
如果不想麻烦，可以在Steam上购买，45块钱解君愁~
……我选择自己编译。

<!-- more -->

如果不想看就直接跳到最后复制代码直接运行。

### 非正式的软件调查

作为一款像素画绘图软件，Aseprite的画风也是“锐化”的：不仅使用像素字体，所有窗口的“颗粒度”也都很大。
美术风格很有意思，用的人也很多。Steam上很多都是用了几十甚至几百小时才评论的用户。

但是，Aseprite跟CJK字符相性不算好，原因有很多。
比如，像素字体的汉字本就是一个挑战，小点阵搭配粗字重更是要了老命；
又比如，该软件自研跨平台C++框架，很多设计一开始就没有考虑CJK兼容性。

以上这些导致的结果就是，虽然软件菜单有汉化，但是图层名称之类的地方都不支持输入法输入。

### 解决方案

#### 让Aseprite支持中文输入法

给开源软件提需求讲究一个基本流程：浏览issues列表->如果没人提到再提交issue。
毫不意外地，早就有人请求添加输入法支持了：[aseprite#1767](https://github.com/aseprite/aseprite/issues/1767)。
最有意思的是，这个issue最早是2018年提交的，核心维护者dacap一而再、再而三地提高该issue的优先级，最终却在2022年取消了assignment状态。四舍五入就是说，放生了。之后开发者方面再没有动作，而用户们时不时会冒出一个问句：“Any update on this?”

好吧，看来输入法还得CJK用户自己搞。
于是我开始着手添加输入法支持。

首先要明确先改什么平台的代码。
我选择先给Windows环境添加输入法支持，不仅用的人多，而且不存在Linux上那种fcitx、ibus、xim三分天下的恐怖感。

至于Mac？我没有Mac。

其次要明确修改哪里的代码。
阅读Aseprite源码，并且用谷歌和chatGPT瞎搜一通之后，我知道要改什么了：[aseprite/laf](https://github.com/aseprite/laf)的`os/win/event_queue.cpp`。Laf是跨平台的兼容层，为上层应用提供统一的窗口相关的接口。

顺便一提，到这里我才发现，Aseprite并非传统意义上的多窗口应用。
类似于flutter，他的各种小窗口都是自己画的，而没有用系统提供的接口创建新窗口。

简而言之，Windows窗体程序通过各种信号接收用户输入，比如窗口的放大缩小、键盘输入之类的。
因此每个Windows窗体都要实现一个处理信号队列的循环，laf里Windows系统相关代码的循环如下：

```cpp
// 信号量变量
MSG msg;
BOOL res = true;

while (res) {
  BOOL res = GetMessage(&msg, nullptr, 0, 0);

  if (res) {
    // 跳过了 KEYDOWN 和 KEYUP 事件！
    if (msg.message != WM_KEYDOWN && msg.message != WM_KEYUP) {
      TranslateMessage(&msg);
    }
    // 没经过 TranslateMessage() 的信号不会被广播。
    DispatchMessage(&msg);
  }
}
```

可以看到，KEYDOWN和KEYUP事件被laf代理掉了。
开发者自信地代理了所有的键盘输入事件，目的是监听各种快捷键。
但是，KEYDOWN也就是按下键盘按键的事件没有被分发（dispatch），Windows系统就认为用户从没输入过什么，因此就不会生成输入法窗口，更不会有后续的输入流程了。

解决问题的方法似乎很简单：只要把被代理掉的KEYDOWN和KEYUP事件还原不就行了。

但是维护者dacap不同意。
他说，要让用户只有在能输入的地方才能输入，也就是输入框之类的地方。
否则程序就监听不到快捷键了。

我觉得这是某种洁癖。
如果只改这一两行，当输入法为英语模式的时候，一切都跟以前一样，只有当切换到中文模式（或者日文韩文）的时候打字才会出现输入法。
许多输入法支持不好的程序都是这样处理的。

比如我的世界。
肯定有人刚进游戏打出过“wwwwwwwwwwwwwwwwwaaasss”这样的字符串吧~

抱怨归抱怨，我还是按维护者的意见改好了代码，只不过他现在装死。

总感觉有股既视感。

哦，就像一个不愿意给学生改论文的研究生导师。

言归正传，目前的代码在`TranslateMessage()`外面又套了一层判断，只有当KEYDOWN事件发生时、用户在可输入区域用输入法输入的时候才进入分支。

```cpp
// 信号量变量
MSG msg;
BOOL res = true;

while (res) {
  BOOL res = GetMessage(&msg, nullptr, 0, 0);

  if (res) {
    if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP) {
      // 我假设只有输入法输入的字符是virtual key
      if (msg.message == WM_KEYDOWN && msg.wParam == VK_PROCESSKEY) {
        // 判断用户是否在可输入区域
        if (IMEManagerWin::instance()->textInput()) {
          TranslateMessage(&msg);
        }
      }
    }
  }
}
```

除此之外，我还实现了一个设置输入法位置的接口。

```cpp
void IMEManagerWin::onStartComposition(HWND hwnd) const
{
  // Set IME form position
  RECT windowRect;
  HIMC imc = ImmGetContext(hwnd);

  if (imc && GetWindowRect(hwnd, &windowRect)) {
    // Get caret relative position to the window
    POINT pos = {
      m_screenCaretPos.x - windowRect.left,
      m_screenCaretPos.y - windowRect.top,
    };
    // Set IME form position: just below the caret
    COMPOSITIONFORM cf = { 0 };
    cf.dwStyle = CFS_FORCE_POSITION;
    cf.ptCurrentPos = pos;
    ImmSetCompositionWindow(imc, &cf);
  }
}
```

这是类`os::win::IMEManagerWin`的公共方法，将在WM_IME_STARTCOMPOSITION事件发生时调用。
其中`m_screenCaretPos`是该类维护的光标在屏幕上的位置，需要在laf上方也就是应用层管理。

#### 自定义字体

搞定输入法之后我发现，有时候输入框里的文字不能对齐。
具体来说就是某些特定的字后面的字可能上蹿下跳。
例如下图中谢字之后所有的逗号都“居中”显示了。
毫无疑问这是字体显示方面的bug。
但是产生原因很复杂，我没有完全搞清楚……
大概知道是因为这些汉字在默认字体里没有，所以要fallback到系统字体（我猜测是微软雅黑）。
我猜测特定的字，尤其是标点的[字距](-advance)计算失败，所以才出现在正中间。

![你好，谢谢，小笼包，再见](/notes/images/你好谢谢小笼包再见.png)

简而言之，bug的产生是因为字体之间的兼容性不好，那么，换一个合适的字体自然可以解决这个问题。
开源的CJK像素字体，以[方舟（ark）像素字体](https://github.com/TakWolf/ark-pixel-font)最为出名。
但是，截止到目前该字体还进展缓慢，只能用[缝合（fusion）像素字体](https://github.com/TakWolf/fusion-pixel-font)替代一下了。
我用的是12px的fusion字体，在我的32寸4K大屏幕上显示效果良好。

![fusion-pixel-font 12px的显示效果](/notes/images/fusion.png)

### 编译Aseprite

~~我感觉添加输入法支持的PR可能会卡一两年，所以在此提供尝鲜版的说明。~~
由于Aseprite的EULA不允许我分发编译好的软件，所以我只能给出尽可能自动的安装脚本。

虽然Aseprite的EULA似乎拦住了一批人，但其实编译起来也没那么困难。
尤其是官方提供了编译脚本，大大降低了难度。
虽然我是踩了不少坑之后才发现的……

#### 编译依赖

编译之前需要做一些准备工作。

- [ ] git
- [ ] MSVC++ toolkits（可通过VS Installer安装）
- [ ] CMake
- [ ] ninja

一定要安装Windows版git，不仅是为了使用git clone指令下载源码，官方的依赖自动安装脚本也需要git bash。
然后需要手动修改一下`build.sh`，目的是开启输入法支持。
在[CMake build的那几行](https://github.com/aseprite/aseprite/blob/main/build.sh#L478-L484)插入`-DLAF_WITH_IME=ON`。

```bash
git clone https://github.com/aseprite/aseprite
git checkout v1.3.15 # 推荐编译发布的版本号而不是直接编译主分支
# 此时编辑build.sh
.\build # build.cmd -> build.ps1 -> build.sh
```

然后根据提示走流程就可以了。

<!-- Finish -->
[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
