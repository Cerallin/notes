---
disableNunjucks: true
mathjax: false
title: 如何使Windows自动切换深色模式以及修改壁纸
tags:
  - VBS
  - PowerShell
  - 注册表
  - Windows
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了基于之前方案改进的一种通过VBS脚本实现Windows登录时自动切换深色模式和壁纸的解决方案。文章详细描述了选用VBS的原因、与注册表交互的实现方法，并分享了相关代码的配置参考及踩坑经验。作者还表达了不再使用VBS进行开发的决心。
date: 2022-01-05 22:25:38
---

> 本文从[之前的解决方案](/2021/12/22/如何使Windows自动切换深浅色模式/)发展而来。在其基础思路指导下新实现了电脑登录时自动切换深色模式，以及壁纸的功能。

本文所描述的代码被托管到了[win-auto-dark](https://github.com/Cerallin/win-auto-dark)。

<!-- more -->

### 前情提要

基于WSL的cron计划任务实现的自动切换深色模式，在实际使用中体现出来一个问题：当电脑休眠或者关机时，计划任务不会被执行。

一种解决思路是，在用户登录时也执行一次脚本。
但是，crontab只有开机hook, i.e., `@reboot`。

查阅资料发现，Windows的任务计划提供了用户登录触发器。
~~真香~~。

好的，接下来该挑战未知的领域了：VBScript (VBS)。

### VBS实现深色模式的自动切换

为什么选VBS呢，因为我记得VBS是Windows系列系统（至少从XP开始）首选的脚本语言，且没有弹窗。

其实在此之前我还试图用bat实现来着，但是写了一部分浑身不适，只好作罢。
如前文所说，切换深色模式只需要修改两个注册表值：`AppsUseLightTheme`和`SysUsesLightTheme`。

### 更换壁纸初尝试

VBS与注册表交互看起来还算简单。
首先创建一个shell交互对象，然后依法调用其`RegWrite`或者`RegRead`方法。
注意，`RegWrite`的最后一个参数制定了注册表项的类型：`REG_DWORD`。

```VB
Set RegObj = WScript.CreateObject("WScript.Shell")
regPath = "HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\Personalize\AppsUseLightTheme"
reg.RegWrite(regPath, True, "REG_DWORD")
```

让我们对象化一下：

```VB
Class MyReg
    ' Set value of a variable of Windows registry.
    Private Function SetVar(var_name, value)
        Dim RegObj, regPath
        Set RegObj = WScript.CreateObject("WScript.Shell")
        regPath = "HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\Personalize\"
        ' It is important to specify "DWORD" as the variable's type
        SetVar = RegObj.RegWrite(regPath & var_name, value, "REG_DWORD")
    End Function

    ' Set value of "AppsUseLightTheme",
    ' using "Let" instead of "Set" means it's not an object.
    Public Property Let AppTheme(theme)
        AppTheme = SetVar("AppsUseLightTheme", theme)
    End Property
End Class
```
### 配置

请参考[GitHub项目的Readme](https://github.com/Cerallin/win-auto-dark)进行配置。

**P.S.** PowerShell实现更换壁纸的代码也在仓库中。

### 附录：VBS踩坑指南 {data-toc-unnumbered=true}

1. VB不区分大小写，不论是关键字、变量名还是类名等等，都不区分大小写。
所以类似Java形式的`WindowObject windowObject = new WindowObject()`是行不通的。
所以，VB中应该写成`Set w_obj = New WindowObject`.

2. VB中对变量进行赋值时，如果是对象则用Set关键字，如上一条所示。如果是其他类型则没有前缀。

3. Class的Property支持Getter和Setter。但是，如果是普通类型赋值，要写Property Let，e.g., 上一节的AppTheme；如果是对象赋值，则需要写Property Set。

4. 函数调用有两种写法：`rtn = func(a, b, c)`，或者`func a, b, c`。应注意，没有返回值的函数或者子程序被调用时不加括号。

~~下次再也不写VBS了~~

[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
