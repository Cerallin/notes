---
disableNunjucks: true
mathjax: false
title: 在CentOS 7上安装GCC 10
tags:
  - C
  - Linux
  - CentOS
categories:
  - 计算机与通信工程
description: >-
  这篇文章介绍了作者在没有管理员权限的CentOS
  7服务器上，为解决CMake编译失败的问题，通过下载依赖并手动编译安装了最新版本的GCC（10.4）。过程中详细描述了依赖的获取、编译选项的配置以及安装步骤，同时分享了实施中的困难与经验。最终成功完成安装，并反思了版本选择和操作过程中的教训。
date: 2022-09-22 10:05:55
---

> 在一台没有管理员权限的CentOS 7上，我想要安装的某个软件需要用到CMake，但是没有安装。
编译CMake的时候报错不支持c++11标准。
`gcc -v`一看，果然，版本4.8。
接下来就是编译安装最新版GCC（10.4）了。

<!-- more -->

### 操作系统

此次的服务器是CentOS 7。

```bash
$ uname -r
3.10.0-862.el7.x86_64
```

### 安装依赖

GCC有三个必选的依赖：[mpc](https://ftp.gnu.org/gnu/mpc/)、[mpfr](https://www.mpfr.org/mpfr-current/)、[gmp](https://gmplib.org/#DOWNLOAD)。
可以联网的服务器可以使用gcc自带的脚本`./contrib/download_prerequisites`下载解压，但是这个服务器无法联网，所以先下载到我自己的电脑上再上传。

阅读脚本可知，所需三个依赖的版本和URL分别为：

```
http://gcc.gnu.org/pub/gcc/infrastructure/gmp-6.1.0.tar.bz2
http://gcc.gnu.org/pub/gcc/infrastructure/mpfr-3.1.6.tar.bz2
http://gcc.gnu.org/pub/gcc/infrastructure/mpc-1.0.3.tar.gz
```

上传脚本到服务器的GCC源码目录，注释掉脚本里下载压缩包的部分（如下所示），然后运行指令`./contrib/download_prerequisites --no-isl --no-verify`。
之后就可以编译了。

```bash
# 第218行
for ar in $(echo_archives)
do
    if [ ${force} -gt 0 ]; then rm -f "${directory}/${ar}"; fi
    [ -e "${directory}/${ar}" ]                                               \
        || ${fetch} --no-verbose -O "${directory}/${ar}" "${base_url}${ar}"       \
        || die "Cannot download ${ar} from ${base_url}"
done
unset ar
```

### 编译GCC

[GCC要编译三次](https://stackoverflow.com/questions/60567540/why-does-gcc-compile-itself-3-times)，真是漫长的等待。

```bash
cd gcc-10.4.0/
mkdir build && cd build

../configure \
  --disable-multilib \
  --prefix=$HOME/.gcc/10.4.0 \
  --enable-languages=c,c++,fortran
```

安装目录指定为`$HOME/.gcc/10.4.0`。
`disable-multilib`意为只编译64位系统相关文件，跳过32位系统相关的编译任务。
`enable-languages`指定只编译c、c++、fortran三种语言的编译器，其实gcc还可以支持这些语言：c, ada, c++, go, brig, d, fortran, objc, obj-c++, gm2。

### 安装GCC

```bash
make install

ln -s $HOME/.gcc/10.2.0 $HOME/.gcc/default
```

`make install`似乎还有一些编译任务要执行，可以指定多线程数加速。
下面一行创建软连接是方便以后切换版本。
之后需要在`.bashrc`里添加一些全局变量：

```bash
export PATH=$HOME/.gcc/default/bin:$PATH

export LD_LIBRARY_PATH=$HOME/.gcc/10.2.0/lib64:$LD_LIRBARY_PATH

export CC=$HOME/.gcc/default/bin/gcc
export CXX=$HOME/.gcc/default/bin/g++
export FC=$HOME/.gcc/default/bin/gfortran
```

### CMake编译选项

当使用CMake编译软件的时候，可以使用如下代码指定编译器：

```bash
cmake \
  -DCMAKE_C_COMPILER=$HOME/.gcc/default/bin/gcc \
  -DCMAKE_CXX_COMPILER=$HOME/.gcc/default/bin/g++ \
  -DCMAKE_Fortran_COMPILER=$HOME/.gcc/default/bin/gfortran \
  -DCMAKE_PREFIX_PATH=$HOME/.gcc/default \
  -B build -S .
```

### 碎碎念

昨晚要安装一个软件，需要先安装C++的YAML库。
编译安装YAML需要CMake，而CMake编译失败了，因为需要gcc支持c++11标准。
但是，gcc 4.8是好多年前的编译器了，早在我刚上高中的时候这个版本还都是最火的gcc版本（当然现在很过时了）。
于是安装gcc。
一开始没看文档，折腾了一晚上。
今早照着别人的教程安装，被依赖版本毒打，因为我安装的版本都太新了，而且据（GCC Wiki）说，如果我编译GCC的时候是动态链接这三个库的，那么之后运行GCC的时候总要动态链接这三个库，也就是需要把他们加入编译器全局变量里。

这显然是不能接受的。
我只是想临时地使用这三个库，不想以后我的所有程序都要链接他们。
最后，对着GCC官方Wiki，终于安装成功了。

事实证明，即使是GCC 4.8，也是可以编译GCC 10.4的！

~~虽然后来在服务器上发现了GCC 11.2，但是事到如今装都装了……~~

<!-- Finish -->
[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
