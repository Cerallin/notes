---
disableNunjucks: true
mathjax: false
title: 使用Perl的XS模块封装C接口
tags:
  - Perl
  - Wrapper
  - OpenCC
categories:
  - 计算机与通信工程
description: >-
  这篇文章介绍了作者开发了一种将OpenCC的C接口封装为Perl模块的方法，以解决小说爬虫中繁体字转换的需求；通过使用XS模块，作者成功创建了一个桥梁，使Perl代码能够调用OpenCC的功能。文章详细讲解了开发过程中的技术要点、目录结构和类型转换原理，并分享了遇到的挑战及解决方案。最终，该项目被应用于实际场景，尽管过程复杂，但作者认为这是一次有趣的编程实践。
---

> OpenCC是一款C++编写的简繁转换软件，支持词汇级别的转换。
虽然OpenCC提供了C、C++、Python、Nodejs的接口，但是没有提供Perl的封装。
本文旨在提供一种编程实践：使用XS模块封装C接口供Perl使用。
在已经安装了OpenCC的计算机上，使用本项目可以方便地在Perl程序中调用OpenCC的接口。
并且，得益于OpenCC设计之简单，很少会有内存泄漏的风险。
该项目已经实际运用到我的互联网文本自动获取与处理程序之中，至少证明它是有人用的^_^

<!-- more -->

### 引言

在Python兴起之前，Perl大概（？）是世界上最流行的脚本语言。
虽然Perl式微，但是平均薪资很高。
以前听信了流言的我抱着试一试的态度学了学。
不得不说，Perl真的很难写。
历经千辛万苦的我最终写出一个爬虫程序，直到现在还在运行。
于是偶尔维护的我勉强记得Perl的语法。

我的爬虫程序最近遇到了一个需求，某个小说繁体字太多，看着头疼。
自然而然地，我想到了OpenCC来解决这个问题。
作为开源领域遥遥领先的繁简转换项目，OpenCC提供了许多常用编程语言的接口，包括C、C++、Nodejs、Python。

不包括Perl。

倒也是意料之中。
毕竟不算常用。

“自己动手丰衣足食”。
我之前就有C和C++互相封装的经验，也看过一点Python封装C接口的官方文档。
虽然当时没有一个契机让我上手操作，所以就搁置了。
略微搜索一下，发现果不其然，Perl也有一套封装C接口的终极方案。
以往用cpan下载包的时候总能看到gcc的编译指令，说明先写C语言然后再提供给Perl的事大家没少干。

Perl提供了一个模块用于封装C接口，名叫XS。
XS这两个字母取自eXtendable Subroutines，其中subroutines就是Perl里的函数，或者说子程序。
该模块的核心是`WriteMakefile`函数，允许开发者定义Makefile生成策略。
与此同时，开发者还需要按照约定组织起特定的文件结构，方便自动生成的Makefile识别源文件。
通过一系列编辑，再加上一系列指令，代码就可以编译为模块文件(.pm)和动态链接库(.so)。

本文以OpenCC为例向大家简单地介绍一种使用其他语言开发Perl模块的通用办法。
项目只用一个下午就写完了（包括不少阅读文档和踩坑的时间），扔在Github上，哪天有心情的时候会去把TODOs做一做。

### OpenCC-Perl


[OpenCC-Perl](https://github.com/Cerallin/OpenCC-Perl)并非OpenCC的Perl实现，而只是对OpenCC的一层封装。
具体而言，该项目需要用户先在目标计算机上安装好OpenCC，然后运行本项目才有意义。
并且，本项目编译安装好之后，原本的OpenCC不能卸载，否则用户在Perl里也不能使用OpenCC的功能了。

#### 使用方法

本项目的使用方法非常简单，new一个类，然后调用接口，就行了。
得益于OpenCC接口本来就相当简洁，本项目也没有几行代码，时间都浪费在一遍又一遍的踩坑和debug上了……
代码示例如下

```perl
use OpenCC;

my $cc = OpenCC->new("s2tw.json");
$converted = $cc->convert("汉字");
print "$converted\n";
```

#### 目录结构

```
├── lib
│   └── OpenCC.pm
├── Makefile.PL
├── OpenCC.xs
└── typemap
```

本项目的有效内容如上所示。
`Makefile.PL`定义了`Makefile`的生成规则。
`lib/OpenCC.pm`和`OpenCC.xs`分别是面向Perl和C的文件。
他们合起来就成为了沟通Perl与C的桥梁。
`typemap`定义了Perl与C的类型转换原则。

### 从C到Perl的桥梁

#### OpenCC的C接口

OpenCC是C++编写的软件，但是也提供了C的接口。
考虑到兼容性，我们使用C接口调用OpenCC的库。

```shell
# 下载并解压Release的源码压缩包，在目录里执行
make && make install
```

此时OpenCC安装目录为`/usr/share/OpenCC`。
该目录下有四个目录，`bin`、`include`、`lib`和`share`，分别代表可执行文件目录、C/C++头文件目录、库文件目录和共享文件目录。
所谓共享文件，对于OpenCC来说就是繁简转换规则相关的文件。

查看`include/opencc/opencc.h`，可以看到接口的核心是`opencc_t`，从`void *`指针类型赋予含义而来。
众所周知，C/C++的指针代表数据存放的地址，通常的项目里使用特定的类型区分各种各样的指针，例如`float *`代表指向浮点数数据的指针，`uint32_t *`指向非负32位整型数的指针。
具体的类型有具体的大小，例如`uint32_t`一定是32位的。
而`void *`类型的指针缺失了它所指向的数据的范围。

作为对外的接口，使用`void *`暴露句柄有两点好处。
一是对外部程序隐藏了自身细节，也就是结构体的定义和数据大小，另外还可以防止外部开发者对不知道长度的指针轻举妄动。

总而言之，在开发Perl模块的时候，我们只需要牢牢记住`opencc_t`是指针类型就可以了。接下来考，请不要忘掉。

#### Perl的XS模块

> If you want to write XS, you have to learn it. Learning XS is very difficult. \
by Steven W. McDougall

第一次看到[XS文档](https://metacpan.org/pod/XS::Tutorial::One)的这一句的时候，我的心里毫无波澜。
因为，ChatGPT老师已经帮我写好了大半。
此时的我对该项目已经有了足够的了解，从始至终完全没有无从下手的感觉。

赞美ChatGPT老师。

前文写到，`lib/OpenCC.pm`和`OpenCC.xs`分别是面向Perl和C的文件。
具体而言，`lib/OpenCC.pm`像是C的头文件，用来暴露给包的使用者，这个文件里需要定义一系列Perl侧的函数或者是类，在内部调用`OpenCC.xs`里约定的C接口，或是把符号（也就是函数名）直接暴露出去。
`OpenCC.xs`使用XS特定的一套规则将C接口封装起来，实现Perl类型和C类型之间的相互转换。
其语法类似K&R C（ANSI C之前的古老的C语言语法），一看就懂，不在此赘述。

接下来我们用一个小例子来进一步说明。
例如，`opencc_convert_utf8()`在原始的C接口里是这样定义的：

```C
char*
opencc_convert_utf8(
    opencc_t opencc,
    const char* input,
    size_t length
);
```

假如`OpenCC.xs`里是这样调用的：
```C
SV*
opencc_convert_utf8(cc, input)
    opencc_t cc
    const char* input
  CODE:
    char* result = opencc_convert_utf8(cc, input, strlen(input));
    RETVAL = newSVpv(result, 0);
  OUTPUT:
    RETVAL
```

那么，`lib/OpenCC.pm`里可以这样暴露接口：
```perl
package OpenCC;

# ...忽略一些类型声明...

# 在此处写出要暴露的函数
our @EXPORT_OK = qw(opencc_convert_utf8);

bootstrap OpenCC; # 加载 OpenCC.xs

1; # Perl模块文件（.pm）特有的结尾
```

或者，由于`opencc_convert_utf8()`已经注册成为Perl的函数，所以可以进一步地封装：
```perl
package OpenCC;

require Exporter;
require DynaLoader;

our @ISA = qw(Exporter DynaLoader);

bootstrap OpenCC;

# 构造函数
sub new {
    my ( $class, $config ) = @_;
    my $self->{_client} = opencc_open($config);
    return bless $self, $class;
}

sub convert {
    my ( $self, $input ) = @_;
    my $result = opencc_convert_utf8( $self->{_client}, $input );
    return $result;
}

1;
```

在上面的代码里，我封装了一个类，该类的`_client`属性即为C接口的`opencc_t`句柄。

### After Story

#### 链接、装载与库

如果安装了我的OpenCC-Perl库，那么计算机上一定存在以下几个链接库文件：

- libmarisa.a
- libopencc.so
- OpenCC.so

其中，前两者是安装OpenCC的时候编译生成的，位于OpenCC目录。
最后一个是用于Perl运行时动态链接的，位于perl模块安装目录。
三者呈现一种从下往上的依赖关系。
除此之外，`OpenCC.so`还依赖于`libstdc++.so`。

在前文提到的“一下午”开发过程中，我主要的时间都用来探索如何编译出用于Perl程序链接的静态链接库了。
最后不得不放弃了这个想法。
这是因为我一直以来都用PAR Packager把Perl可执行程序打包然后部署到服务器上。
所以自然而然地想要静态链接，好方便我打包程序。

但事实证明我错了。

使用PAR Parser打包的程序根本不是一个完完全全彻彻底底的可执行程序，而是一个把所有文件都打包到一起的类似tarball的东西，无论是代码源文件还是动态链接库文件，都在里面。
所以我的思路错了。

不再执着于编译静态库之后，程序很快就写好了。

#### 回到开始

一开始我的目的仅仅是在我的爬虫中添加一段代码。
这种小小的需求一般是如何实现的呢？

答案是，调用系统相关函数，新建一个进程，运行OpenCC自带的程序即可。
前文章节[OpenCC的C接口](#opencc的c接口)提到，OpenCC有一个`bin`目录。
该目录里有名为`OpenCC`的可执行文件，从stdin读取，并将转换结果输出到stdout，非常适合使用管道传输数据。
实际上，许多filter都是以管道的思路实现的。

但是，我没有选择这个方案。
虽然开发难度降低了不少，但是这种方案有一个最大的问题：新建进程的开销大以至于运行速度显著变慢。
由于我的爬虫采用轮询机制，每隔10分钟把订阅的小说挨个检查一遍。
非异步的网络请求已经很耗时间了，如果每篇内容的处理速度平均慢上半秒，很可能在极端情况下遇到两个调度相遇的尴尬情况。
处理这种情况远比开发OpenCC的Perl脚本要麻烦，而且我不打算重写成异步的请求，为了不给服务器太大的压力。

### 总结

总的来说，OpenCC足够小，非常适合练手，一下午就写完了，这还是因为我执着于静态编译多花了几个小时。
如果未来有时间可以加点测试用例，或者实现一套完整的模块，而不需要提前安装OpenCC。
但是XS实在是太难写了，过于小众以至于连代码高亮都没有……
所以未来不会有机会再用其他语言开发Perl模块了，除非加钱。

加钱！

<!-- Finish -->
[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
