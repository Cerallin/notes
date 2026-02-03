---
disableNunjucks: true
mathjax: true
title: 使用VSCode的学术写作
tags:
  - LaTeX
  - VSCode
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了LaTeX在学术写作中的效率问题并对比了其与Word的优劣，重点阐述了在Ubuntu或WSL2环境下安装和配置LaTeX的方法，包括TexLive安装、环境变量设置及VSCode插件的使用。文章还提供了编译方案设置和优化建议，帮助用户更便利地完成LaTeX写作。最后引用了一项对文档编辑系统效率比较的学术研究。
date: 2022-01-09 16:17:23
---

---
bibliography: source/_bibs/使用VSCode的学术写作.bib
---

> 有研究[@knauff2014efficiency]表明，LaTeX用户比Word用户进行学术写作更慢，在相同的时间内写的文本更少，并产生了更多的排版、拼写、语法和格式错误。
但是，LaTeX也有其Word无法比拟的有点：排版和内容分离，更便于版本管理和格式切换。
本文简单介绍如何在Ubuntu或者WSL2环境下进行LaTeX写作。

<!-- more -->

### 下载安装LaTex

#### 下载TexLive

最简单的办法是使用`apt`安装。但该版本TexLive往往不是最新的。
最新版本（2021）的TexLive可以从清华源下载并安装。

```sh
aria2c https://mirrors.tuna.tsinghua.edu.cn/CTAN/systems/texlive/notes/images/texlive2021-20210325.iso
```

下载CLI GUI组件库，这样在接下来安装的时候就有图形界面了。
```sh
sudo apt-get install perl-tk
```

#### 安装TexLive

挂载ISO，运行安装程序，然后卸载。
挂载路径不推荐设置成`/mnt`，可以在家目录里随便创建一个目录然后挂载。

```
sudo mount -o loop texlive.iso /mnt
cd /mnt
sudo ./install-tl -gui
cd -
sudo umount /mnt
```

Windows的安装更简单了，双击ISO挂载，直接运行安装程序，然后右键卸载。

#### 设置环境变量

在你的`~/.bashrc`（zsh的话是`~/.zshrc`）文件里添加如下几行：

```sh
export PATH=/usr/local/texlive/2021/bin/x86_64-linux:$PATH
export MANPATH=/usr/local/texlive/2021/texmf-dist/doc/man:$MANPATH
export INFOPATH=/usr/local/texlive/2021/texmf-dist/doc/info:$INFOPATH
```

#### 测试安装是否成功

```sh
tlmgr --version
pdftex --version
xetex --version
luatex --version
```

### 安装VSCode的LaTex插件

[LaTeX Workshop](https://marketplace.visualstudio.com/items?itemName=James-Yu.latex-workshop)

### 配置VSCode

在你的全局user settings里添加如下内容：
```json
{
    "latex-workshop.view.pdf.viewer": "tab",
    "latex-workshop.latex.recipe.default": "lastUsed"
}
```

其中`view.pdf.viewer`指定预览文件的浏览器，可以是vscode内置tab，也可以是browser。
`latex.recipe.default`指定默认使用的latex编译方案，有first和lastUsed两种取值。

下面的代码定义了四个编译方案，两个给中文，两个给英文，两个给带参考文献的交叉引用，两个没有引用。

嗯，我相信你听懂了。

```json
{
    "latex-workshop.latex.recipes": [
        {
            "name": "xelatex",
            "tools": [
                "xelatex"
            ]
        },
        {
            "name": "latexmk",
            "tools": [
                "latexmk"
            ]
        },
        {
            "name": "pdflatex -> bibtex -> pdflatex*2",
            "tools": [
                "pdflatex",
                "bibtex",
                "pdflatex",
                "pdflatex"
            ]
        },
        {
            "name": "xelatex -> bibtex -> xelatex*2",
            "tools": [
                "xelatex",
                "bibtex",
                "xelatex",
                "xelatex"
            ]
        }
    ]
}
```

**P.S.** 推荐在LaTeX项目的workspace里将`files.autoSave`设置成`onFocusChange`。
虽然LaTeX编译总是很快，但在电池供电场景，频繁编译显然会导致耗电加剧。

[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}

### 参考文献 {data-toc-unnumbered=true}
