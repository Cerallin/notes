---
disableNunjucks: true
mathjax: true
title: 使用LaTeX与MuseScore排版乐谱
tags:
  - LaTeX
  - MuseScore
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了作者利用中秋假期整理自己创作的乐谱，通过MuseScore和LaTeX结合生成排版整齐的PDF用于打印装订，并解决了乐谱排序、页面格式调整及批量导出等技术问题。文章详细讲解了使用LaTeX添加页眉页脚、自定义TOC、调整单双页间距以及通过XML格式优化MuseScore文件的过程。最后，作者总结了项目的成果与不足，并分享了相关LaTeX模板的GitHub链接。
date: 2022-09-13 15:17:24
---

> 最近心血来潮，趁着摸鱼时间和中秋假期把迄今为止我扒谱的所有曲目整理出来，挑选我最喜欢的几首，打算装订成册。
本想如果MuseScore支持的话，打算全部复制粘贴到一个文件里，生成PDF然后打印。
但查了查发现似乎没有这种操作，遂改为用LaTeX排版。

<!-- more -->

### 可行性分析

#### LaTeX与MuseScore的协同

初步的构想是MuseScore生成乐谱，LaTeX负责整合PDF、控制页脚页数、封面和目录。

在LaTeX中使用`pdfpages`包可以添加PDF文件，代码如下，其中指定的`pagecommand`选项可以添加LaTeX中定义的页眉页脚。
如果能够隐藏MuseScore生成的PDF的页眉页脚，就不会与LaTeX冲突，因而可以自定义页眉与页脚。

```latex
\usepackage{pdfpages}
\includepdf[pages=-, pagecommand={}]{./path/to/example.pdf}
```

#### 双面翻页

为了装订成册，最终LaTeX生成的PDF必须是双面翻页的书本格式，也就是说，纸张内侧（奇数页左和偶数页右）的空白要比外侧更大一些。

### 关键技术问题

#### 章节与目录

为了在开头自动生成一个目录（TOC），我尝试使用`\section`控制章节，但发现随后插入的乐谱与章节页之间的空白十分浪费，所以希望可以在不显示章节的情况下也能生成TOC。
搜索资料之后，设计出`\fakesection`命令如下，可以在正文中不显示章节名的情况下设置TOC中的章节名。
其中`\numberline{}`取消了TOC中章节前面的数字。

```latex
\newcommand{\fakesection}[1]{
\par\refstepcounter{section} % 维护 section 计数器，虽然感觉没有必要……
\sectionmark{#1}
\addcontentsline{toc}{section}{\protect\numberline{}#1}
}
```

#### 页眉与页脚

首先要取消MuseScore的页眉页脚。
查阅MuseScore论坛可知，MuseScore可以导入导出乐谱样式，一种扩展名为`mss`的XML配置文件。
部分选项如下：

```xml
<showHeader>0</showHeader>
<showPageNumber>1</showPageNumber>
<showPageNumberOne>0</showPageNumberOne>
<showFooter>1</showFooter>
```

显然0和1就是否和是的意思。
使用如下所示代码可以批量修改乐谱样式。

```
mscore -S mystyle.mss example.mscz -o output.mscz
```

#### 单双页与页数设置

为了节约空间，两份乐谱之间不应该有空白页。
而MuseScore生成乐谱时第一页总是奇数页，应当根据上一份乐谱设置单/双页开始。

查阅资料可知，软件中有指定起始页的功能，不过一个一个鼠标去点实在是太麻烦了。
有没有批量修改的方案呢？

答案是有的。

虽然MuseScore保存时默认的格式是mscz（压缩了的MuseScore文件），实际上还有另一个未压缩的，以XML格式存储信息的mscx格式。
可以看到其中存储了样式和所有音符的信息。

而首页页数，通过`<page-offset>`标签控制（默认为0）。

### 解决方案

#### 乐谱按时间顺序排序

18年以来，我的乐谱在好几个电脑之间传递，有些乐谱的确切的修改时间已经不可考了。
还有些乐谱在漫长的时间里经历过几次勘误，修改时间相对于于初次定稿时间落后了太多。

好在文件里其实保存了乐谱的创建时间，接下来的排序就以此为基准。

`mscx`格式的MuseScore文件本质上是XML，元信息中保存的创建时间格式如下：

```XML
<metaTag name="creationDate">2022-08-09</metaTag>
```

首先扫描所有乐谱文件然后存储列表到`list.txt`。

```bash
getCreationDate() {
    local file=$1

    grep 'creationDate' $file |     # 查找metaTag
        sed 's_.*">__g;s_<.*>__g'
}

getCreationDateWithFormat() {
    local file=$1

    grep 'creationDate' $file |     # 查找metaTag
        sed 's_.*">__g;s_<.*>__g' |
        xargs date +"%h %d, %Y" -d
}

exec 3>list.txt

ls -haltr *.mscx | awk '{print $9}'| while read file; do
    name=${file%%.*}    # 去除扩展名
    pdf=$name.pdf
    name=${name//_/ }   # 下划线转空格
    echo "$name|$(getCreationDateWithFormat $file)|$pdf|$(getCreationDate $file)" >&3
done
```

然后使用如下指令重新排序：

```bash
awk -F '|' '{print $NF,"|",$0}' list.txt | sort | awk -F '|' '{print $4}'
```

#### MuseScore批量导出PDF

MuseScore支持一次性执行批量任务。
相比于shell批处理的好处是不需要反复创建销毁MuseScore的Qt窗体，节约开销，加快执行速度。

但是本次不可以使用批量任务，因为导出一份乐谱之前，需要知道它的第一页究竟是奇数页还是偶数页。

设置`page-offset`的函数如下，美中不足的一点是多次执行会造成一堆空格。

```bash
set_offset() {
    sed -i 's_^<page-offset.*$__' "$1"
    ed "$1" <<EOF
8a
<page-offset>$offset</page-offset>
.

wq
EOF
}

```

ed是一种命令行文件编辑器，sed是其升级版。ed部分代码的意思是在第8行后插入一行。

### 总结的碎碎念

折腾了一周多的时间（虽然一半时间用于修缮谱子），终于整理出一份80页A4纸打印的曲谱集，本来还想在每章开头写点什么，最终还是懒得写。
但是现在章节页实在是太空了，电脑上看的时候还不觉得，打印出来之后才觉得难受……
目录字体调得太大了，显得有些呆笨。
乐谱标题太靠上，上边的空间过于紧张。
外侧间距留小了，没有考虑到装订的时候会切一部分下去，失算了。
但是如果外间距再小一点，好多谱子就会多出一行，浪费很多的纸面空间……目前还没想好怎么办。

总而言之，LaTeX模板已经上传至GitHub。

[项目地址](https://github.com/Cerallin/scorebook-template)

<!-- Finish -->
[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
