---
disableNunjucks: true
mathjax: true
title: 使用MATLAB绘制3D图形
tags:
  - MATLAB
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了作者在使用MATLAB进行画图和数据处理中的一系列操作，包括生成网格数据、计算因变量值及绘制3D图，同时分享了定义函数、数值求解方程以及判断数是否为实数的技巧。文中强调了MATLAB自带优化功能的高效性，并提供了具体代码示例和注意事项。整体内容侧重于实际操作和个人经验的记录。
date: 2023-06-01 14:42:22
---

> 近日帮别人用MATLAB画图，记录一下。
MATLAB版本为最新的R2023a。
毕竟学校买了正版，不用白不用。

<!-- more -->
### 使用两个向量生成meshgrid

```MATLAB
% meshgrid 的两个参数长度需保持一致
[gamma_arr, nh_arr] = meshgrid( ...
    0.077562 : 4E-7 : 0.0775699999, ...
    3.8794E19 : 4.5E14 : 3.88029999E19 ...
);
```

假设meshgrid的参数中两个向量长度为N，则运行结果是得到两个NxN的矩阵X和Y，例子如下：
```
>> [X, Y] = meshgrid(1:4, 5:8)

X =

     1     2     3     4
     1     2     3     4
     1     2     3     4
     1     2     3     4


Y =

     5     5     5     5
     6     6     6     6
     7     7     7     7
     8     8     8     8
```

[这样一来，$\forall i, j \in (0, N)$ 有 $(x_i, y_j)$ 表示二维直角坐标系上的一个点。]{.display-block .text-indent-none}

### 计算因变量的值

```MATLAB
% 预分配矩阵内存
F_mat = zeros(length(gamma_arr), length(nh_arr));

% 愚蠢地逐个计算
for i = 1 : length(gamma_arr)
    % 用 parfor 小小地加速一下
    parfor j = 1 : length(nh_arr)
        gamma = gamma_arr(i, j);
        nh = nh_arr(i, j);
        % F 为我定义的自由能函数，其中 jnkbt, N, NC 为常量
        F_mat(i, j) = F(gamma, nh, jnkbt, N, NC);
    end
end
```

这里我对`F_mat`的计算方法不是很满意。
虽然可以用parfor并行计算，但是还是想用类似于闭包的东西写得简洁一些（并且相信MATLAB做足了优化，不需要我手动并行了）。

### 3D作图

![MATLAB绘制的3D曲面](/notes/images/surf-matlab.bmp)

```
% surf 可以作出一个平面
surf(gamma_arr, nh_arr, F_mat)
```

[自此图就画好了。]{.display-block .text-indent-none}

### 附录：一些常见技巧 {data-toc-unnumbered=true}

#### 定义一个函数 {data-toc-unnumbered=true}

```MATLAB
function S = SUM(a, b)
    S = a + b;
end
```

[其中`S`是函数的返回值，当函数执行完毕，S的值就是函数的返回值。
实际上，函数名称和函数返回变量可以重名，例如可以把第一行修改为`function F = F(a, b)`。]{.display-block .text-indent-none}

#### 求解方程的数值解 {data-toc-unnumbered=true}

```MATLAB
syms x;

% Equation
eq = x*x + 2x - 3;

% default
res = vpasolve(eq == 0, x);
res = vpasolve(eq == 0, x, 100); % in which 100 is an initial guess
```

首先需要定义若干需要求解的符号变量（用于解方程的符号而不是真正的编程变量），然后调用`vpasolve`即可。
顺便一提`solve`可以求解析解，但是一旦求不出来，就会先报一个warning然后fallback到`vpasolve`求解。

#### 判断一个数是否为实数 {data-toc-unnumbered=true}

MATLAB提供了一个函数`isreal`用于判断一个数是实数或者复数。

```MATLAB
r = 1;
nr = 1 + 3i;

isreal(r);  % 1(true)
isreal(nr); % 0(false)
```

[[文档](https://ww2.mathworks.cn/help/matlab/ref/isreal.html#description)指出，虚部为0的数会被认为是复数：]{.display-block .text-indent-none}

```MATLAB
c = complex(1) % 1.0 + 0.0i
isreal(c) % 1(true)
```

[`vpasolve`的返回值可能是虚部为0的复数，因此需要改用虚部是否为0判断是否为实数：]{.display-block .text-indent-none}

```MATLAB
c = complex(10) % 10.0 + 0.0i
imag(c) != 0 % 1(true), i.e., is real.
```

<!-- Finish -->
[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
