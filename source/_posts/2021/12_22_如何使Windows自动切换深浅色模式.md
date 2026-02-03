---
disableNunjucks: true
mathjax: true
title: 如何使Windows自动切换深浅色模式
tags:
  - WSL
  - Windows
  - 注册表
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了作者为了解决晚上浅色模式刺眼问题，设计一个自动切换浅色与深色模式的解决方案。文章分析了需求、可行性，以及Windows与WSL系统下的具体实现方法，包括修改注册表和任务调度机制。最后提出了通过编写脚本和使用Cron或任务计划程序实现自动切换的思路和伪代码。
date: 2021-12-22 12:16:05
---

> 晚上，浅色模式要亮瞎我的双眼。手动切换深色太麻烦，现考虑自动化实现。

<!-- more -->

### 需求分析

程序应完成以下功能：

- 每日晚上定时自动从浅色转换为深色；
- 每日上午定时自动从深色转换为浅色；
- 电脑开机时检查当前颜色设置并自动切换。

### 可行性分析

#### 系统环境

Windows 10家庭版。
WSL: Ubuntu.

#### 手动实现

手动设置选项路径为：

1. 设置
2. 个性化
3. 选择颜色
4. （浅色/深色/自定义）

#### 自动实现

![注册表编辑器](/notes/images/注册表.png){#fig:vars}

查阅资料可知，通过修改注册表值可以实现命令行下的颜色切换。
例如以下代码（Powershell）可以将`默认App模式`切换为深色。

```powershell
reg.exe add "HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\Personalize" `
  /v AppsUseLightTheme ` # 变量名
  /t REG_DWORD `
  /d 0 `
  /f
```

其中`AppsUseLightTheme`是所要修改的变量名。
通过注册表编辑器可以看到，需要修改的变量为`AppsUseLightTheme`和`SystemUsesLightTheme`，如[@fig:vars]所示。

Windows系统有个叫“任务计划程序”的东西，可以实现定时执行脚本。
听起来和Linux的cron很像，不过该方法能够设置的最小的时间间隔就是一天，远没有cron的1分钟强大。
如果使用该功能实现我的需求，则需要三条任务计划规则才能实现，好处是每条规则一行就能实现，也不需要自定义复杂的脚本。
如果使用WSL的cron，则每小时调用我的脚本程序就好。
代价是需要自己实现被调用的脚本，但好处是可以通过修改脚本的方式调整触发事件。

### 伪代码

```vb
' 黑夜模式为 dark_time 到第二天的 light_time
dark_time := 21:30
light_time := 6:30
cur_theme_mode := current theme mode

now := current time.

SUB sync_theme_mode(cur_mode, new_mode)
    IF cur_mode equals new_mode
    ' comment
    THEN
        RETURN
    ELSE
        Sync current mode to new mode.
    END IF
END SUB

IF now < dark_time AND now > light_time
THEN
    sync_theme_mode(cur_theme_mode, light_theme_mode)
ELSE
    sync_theme_mode(cur_theme_mode, dark_theme_mode)
END IF
```

### Shell代码

Cron部分：

```sh
# 注意必须是绝对路径，且要给脚本文件加执行权限
0 * * * * /path/to/auto_dark_mode.sh
@reboot /path/to/auto_dark_mode.sh
```

`/path/to/auto_dark_mode.sh`:
****
```sh
#!/bin/bash

# Calc times
time_dark=1230 # 20:30 20 * 60 + 30
time_light=390 # 6:30 21 * 60 + 30
time_now=$(date +'%H*60+%M' | bc)

# Get current theme mode
query() {
    /mnt/c/Windows/system32/reg.exe query \
        "HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\Personalize" \
        /t REG_DWORD \
        /f "LightTheme" | tr -d '\r' | # 去除 \r 以适配 Linux 系统
        grep "$1" | awk '{print $3}'
}
cur_app_mode=$(query 'AppsUseLightTheme')
cur_sys_mode=$(query 'SystemUsesLightTheme')

set_mode() {
    local var="$1"
    local -i mode="$2"

    /mnt/c/Windows/system32/reg.exe add \
        "HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\Personalize" \
        /v "$var" \
        /t REG_DWORD \
        /d $mode \
        /f
}

sync_theme_mode() {
    new_mode="$1"

    if [ "$cur_app_mode" != "$new_mode" ]; then
        set_mode "AppsUseLightTheme" $new_mode
    fi

    if [ "$cur_sys_mode" != "$new_mode" ]; then
        set_mode "SystemUsesLightTheme" $new_mode
    fi
}

get_target_theme_mode() {
    if [ $time_now -lt $time_dark -a $time_now -gt $time_light ]; then
        printf "0x1"
    else
        printf "0x0"
    fi
}

sync_theme_mode $(get_target_theme_mode)
```

[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
