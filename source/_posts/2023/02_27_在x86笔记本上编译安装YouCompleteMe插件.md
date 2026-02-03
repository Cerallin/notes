---
disableNunjucks: true
mathjax: false
title: 在x86笔记本上编译安装YouCompleteMe插件
tags:
  - clang
  - Vim
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了一位用户在一台1核2线程的小笔记本电脑上安装AntiX系统并配置Vim作为编辑器的过程，主要解决了YouCompleteMe插件对clangd
  v15+的依赖问题。用户自行编译了LLVM工具链以获取适配的clangd版本，同时克服了低配置设备在编译过程中的性能限制。最终，作者通过调整配置文件成功完成了YouCompleteMe的安装和适配以实现C语言的代码自动补全功能。
date: 2023-02-27 20:09:44
---

> 最近入手了一台x86架构的小笔记本电脑，CPU是1核2线程。
想作为临时维护机，平时连接其他计算机，或者临时开发。
于是安装了AntiX系统（为数不多还在维护x86软件仓库的发行版），并且准备使用Vim作为编辑器（VSCode开销太大装不上）。

<!-- more -->

### Vim的包管理

使用`vim-plug`管理Vim插件，使用方法与Vundle类似。

```vim
call plug#begin()

Plug 'vim-airline/vim-airline'
Plug 'scrooloose/nerdtree'

Plug 'ycm-core/YouCompleteMe', { 'on': 'NERDTreeToggle' }

" ...

call plug#end()
```

### 安装YouCompleteMe

使用`:PlugInstall`安装的只不过是YouCompleteMe的外壳。
为了实现C语言的自动不全，还需要合适的后端：clangd v15+。

clangd是clang的language server，在clang-13之后官方不再提供x86二进制包。
而YouCompleteMe所需的clangd最低版本是v15.0.1（目前的lts是v15.0.7）。
无论如何，都需要自行编译clang了。

### 编译clang工具链

#### 下载llvm-project

下载

```shell
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-15.0.7/llvm-project-15.0.7.src.tar.xz
```

然后解压

```shell
tar xf llvm-project-15.0.7.src.tar.xz
```

#### CMake选项

进入源代码文件夹，使用cmake生成编译选项。

```shell
cd llvm-project-15.0.7.src
cmake -S llvm \
    -B build \
    -G "Unix Makefiles" \
    -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
    -DMAKE_BUILD_TYPE=X86 \
    -DLLVM_TARGETS_TO_BUILD=X86 \
    -DMAKE_INSTALL_PREFIX=$HOME/.llvm
```

上面的cmake指令可以编译llvm、clang和clangd（包含在clang-tools-extra里），最后安装到`$HOME/.llvm`目录。
为了编译速度，我舍弃了X86以外的所有架构，因为1核2线程实在是难以忍受地慢，而且除X86以外的交叉编译显然在其他计算机上运行。

**注意** 编译clang时内存使用量最高可达2.5G。一开始我大意了，以为笔记本2G足矣，重启一次之后不得不又开了2G swap。

#### 编译 && 测试 && 安装

当前目录是`llvm-project-15.0.7.src`。

```shell
cd build
make -j2
make check # 可选
make install
```

![编译运行结果图。](/notes/images/screenshot-clang-test.jpg){#fig:compile-result}

如[@fig:compile-result]所示，编译运行测试的时间就要将近5个小时，而编译则需要1天零13小时。实际上`make check`运行报错了，有51个test case没有通过。考虑到clang早已放弃对x86的支持，我猜测没通过的test case里有一部分是x86平台不适用的代码。

### 编译安装YouCompleteMe

上文提到，llvm被安装到`$HOME/.llvm`目录，编译YouCompleteMe的时候需要指定llvm路径，需要把下面的指令里的`$HOME/.llvm`修改为你的llvm的安装路径。

```shell
cd ~/.vim/plugged/YouCompleteMe
EXTRA_CMAKE_ARGS="-DPATH_TO_LLVM_ROOT=$HOME/.llvm" \
    python3 ./install.py --clangd-completer --verbose
```

`--verbose`用于显示CMake的编译进度，可以不加。
该命令执行完成会报错`No pre built Clang 15.0.1 binaries for this system.`，无视。

### 配置YouCompleteMe

为了让YouCompleteMe识别到clangd的安装位置，需要在`.vimrc`里加上一行：

```vim
let g:ycm_clangd_binary_path = "/path/to/clangd"
```

其中`/path/to/clangd`替换为clangd的路径。

<!-- Finish -->
[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
