---
disableNunjucks: true
mathjax: true
title: Ewald Summation的矩阵优化
tags:
  - MATLAB
categories:
  - 材料科学与工程
description: >-
  这篇文章介绍了一种基于矩阵运算的Ewald
  sum计算策略，通过将复杂的向量运算和求和形式改写为矩阵形式，降低了浮点数运算的次数并支持CUDA加速。文章详细说明了能量短程部分、长程部分和自能修正项的矩阵计算方法，并通过预先计算Ewald矩阵实现系统模拟的优化。实验对比表明，优化后的方法在MATLAB和C++实现中均显著提升了计算速度和效率。
date: 2024-02-05 10:11:20
---

> 为了快速地计算某Ewald sum，现推导一种基于矩阵的计算策略，一种很新的算法。
在时间复杂度方面本策略没有什么提升，还是$O(n^2)$。
但是，比起传统方法，本策略的FLOPs大幅降低，并且支持cuda加速。

<!-- more -->

<div class="display-none">
$$
% 定义常量
\def\Nr{{\text N_r}}
\def\Nk{{\text N_k}}
\def\Nq{{\text N_q}}
% 简写
\def\dag{{\dagger}}
\def\T{{\text{T}}}
\def\tr{{\text{tr}}}
\def\abs{{\text{abs}}}
\def\triu{{\text{triu}}}
\def\erfc{{\text{erfc}}}
\def\diag{{\text{diag}}}
\def\real{{\text{real}}}
\def\vec#1{{\boldsymbol{#1}}}
\def\matsum#1{{\left\langle {#1} \right\rangle}}
% 矩阵小妙招
\newcommand{\matsize}[2]{\text{#1}\times\text{#2}}
\newcommand{\matspace}[2]{\mathbb{R}^{{{#1}\times{#2}}}}
\newcommand{\simpmat}[3]{
  \begin{bmatrix}
    #1_{11} & #1_{12} & \cdots & #1_{1#3} \\
    #1_{21} & #1_{22} & \cdots & #1_{2#3} \\
    \vdots & \vdots & \ddots & \vdots \\
    #1_{#21} & #1_{#22} & \cdots & #1_{#2#3} \\
  \end{bmatrix}
}
\newcommand{\vecmat}[2]{
  \begin{bmatrix}
    \vec{#1_1} \\
    \vec{#1_2} \\
    \vec{#1_3} \\
    \vdots \\
    \vec{#1_#2}
  \end{bmatrix}
}
$$
</div>

### 本文涉及的矩阵运算

#### 向量的数量积

一个$n$维的向量实际上就是大小为$\matsize{1}{N}$的矩阵，向量点乘可以定义为一个行向量与列向量的乘积。
设$\vec{a} = [a_1, a_2, a_3, \dots, a_n]$，$\vec{b} = [b_1, b_2, b_3, \dots, b_n]$，则$a$与$b$的数量积

$$
\vec{a} \vec{b}^\T =
\begin{bmatrix}
  a_1 & a_2 & a_3 & \dots & a_n
\end{bmatrix}
\begin{bmatrix}
  b_1 \\
  b_2 \\
  b_3 \\
  \vdots \\
  b_n
\end{bmatrix}
= \sum_{i=1}^n a_i b_i .
$$

#### 矩阵乘法

设$A$是一个$\matsize{M}{N}$的矩阵，$B$是一个$\matsize{L}{M}$的矩阵，矩阵乘法定义为
$$
\begin{aligned}
B A
&= \simpmat{b}{L}{M} \cdot \simpmat{a}{M}{N} \\
\\
&=
\begin{bmatrix}
  \sum_{i=1}^M b_{1i} a_{i1} & \sum_{i=1}^M b_{1i} a_{i2} & \cdots & \sum_{i=1}^M b_{1i} a_{iN} \\
  \sum_{i=1}^M b_{2i} a_{i1} & \sum_{i=1}^M b_{2i} a_{i2} & \cdots & \sum_{i=1}^M b_{1i} a_{iN} \\
  \vdots & \vdots & \ddots & \vdots \\
  \sum_{i=1}^M b_{Li} a_{i1} & \sum_{i=1}^M b_{Li} a_{i2} & \cdots & \sum_{i=1}^M b_{Li} a_{iN} \\
\end{bmatrix}
\end{aligned}
$$
其中第$i$行$j$列的项的值为$\sum_{k=1}^M b_{ik} a_{kj}$。

#### 矩阵的Hadamard积

设$A$，$B$均为$\matsize{M}{N}$的矩阵，A与B每个相同位置元素的乘积被称为Hadamard积，记作$A \odot B$，矩阵之间的Hadamard积的逆运算，即矩阵元素分别做除法，记作$A \oslash B$。
矩阵A与B的Hadamard积
$$
\begin{aligned}
A \odot B &= \simpmat{a}{M}{N} \odot \simpmat{b}{M}{N} \\
\\
&=
\begin{bmatrix}
  a_{11}b_{11} & a_{12}b_{12} & \cdots & a_{1N}b_{1N} \\
  a_{21}b_{11} & a_{22}b_{22} & \cdots & a_{2N}b_{2N} \\
  \vdots & \vdots & \ddots & \vdots \\
  a_{M1}b_{M1} & a_{M2}b_{M2} & \cdots & a_{MN}b_{MN} \\
\end{bmatrix}
\end{aligned}
$$

#### 矩阵的数值平方

设矩阵$A$是一个$\matsize{M}{N}$维的向量，定义矩阵的一种运算
$$
|A \otimes A| =
\vecmat{a}{N} \otimes \vecmat{a}{N}
=
\begin{bmatrix}
  \vec{a_1} \cdot \vec{a_1} \\
  \vec{a_2} \cdot \vec{a_2} \\
  \vec{a_3} \cdot \vec{a_3} \\
  \vdots \\
  \vec{a_N} \cdot \vec{a_N}
\end{bmatrix}
=
\begin{bmatrix}
  \vec{a_1}^2 \\
  \vec{a_2}^2 \\
  \vec{a_3}^2 \\
  \vdots \\
  \vec{a_N}^2
\end{bmatrix}
$$
……就称为矩阵的数值平方好了。
其中$\vec{a_1}, \vec{a_2}, \vec{a_3}, \dots, \vec{a_N}$均为维度为N的行向量。

#### 矩阵求和

对于大小为$\matsize{M}{N}$矩阵$A$，规定$\matsum{A}$表示矩阵$A$所有元素的和，即$\matsum{A} = \sum_{i=1}^M\sum_{j=1}^N a_{ij}$。

#### 矩阵上的函数

对于大小为$\matsize{M}{N}$的矩阵$A$，如果其所有元素均在函数$F(x)$的定义域内，则
$$
\begin{aligned}
F(A) &= F\left(\simpmat{a}{M}{N} \right) \\
\\
&=
\begin{bmatrix}
  F(a_{11}) & F(a_{12}) & \cdots & F(a_{1N}) \\
  F(a_{21}) & F(a_{22}) & \cdots & F(a_{2N}) \\
  \vdots & \vdots & \ddots & \vdots \\
  F(a_{M1}) & F(a_{M2}) & \cdots & F(a_{MN}) \\
\end{bmatrix}
\end{aligned}
$$

#### 厄米矩阵与共轭转置

如果复矩阵$H$的共轭转置等于它自己，即$H^\dag = H$，则称$H$是一个厄米矩阵。

现证明厄米矩阵$H$的对角元素组成的矢量$\diag(H)$等于$H$的实部的对角元素组成的矢量$\diag(\real(H))$。

首先将厄米矩阵$H$分解为一个实矩阵和一个只有纯虚数和0的矩阵的和
$$
H = H_r + H_i
$$ {\#eq:hermitian-def}
其中$H_r$为$H$的实部，$H_i$为虚部。
厄米矩阵的对角元素必须等于自己的复共轭，因此都是实数。
所以$H_i$是一个反对称矩阵，满足$H_i^\T = - H_i$。
厄米矩阵$H$的对角元素组成的矢量$\diag(H)$可以拆成两个向量的和：
$$
\begin{aligned}
\diag(H) &= \diag(H_r + H_i) \\
&= \diag(H_r) + \diag(H_i) .
\end{aligned}
$$
因为矩阵$H_i$的对角元素都是0，所以$\diag(H_i) = 0$。
因此有
$$
\diag(H) = \diag(H_r) = \diag(\real(H)) .
$$ {\#eq:hermitian-diag}
其中$\real(H)$表示复矩阵$H$的虚部。

[[**引理1**]{.pr-6} 现有大小相等的实对称矩阵$A$与厄米矩阵$H$，则$\tr(A H) = \tr(A \cdot \real(H))$。]{#lemma-1 .display-block .text-indent-none .mt-4}

[**证明**]{.display-block .text-indent-none}

首先矩阵$A$乘以矩阵$H$可以分解为
$$
A H = A (H_r + H_i) = A H_r + A H_i .
$$
根据矩阵的迹的性质有
$$
\begin{aligned}
\tr(A H) &= \tr(A (H_r + H_i)) \\
&= \tr(A H_r) + \tr(A H_i) .
\end{aligned}
$$ {\#eq:hermitian-real-1}
因为反对称矩阵$H_i$满足$H_i^\T = -H_i$，从而有
$$
(A H_i)^\T = H_i^\T A^\T = - H_i A \\
$$
两边取矩阵的迹得
$$
\begin{aligned}
\tr([A H_i]^\T) &= \tr(A H_i) , \\
\tr(- H_i A) &= - \tr(H_i A) = - \tr(A H_i) .
\end{aligned}
$$
联立得$\tr(A H_i) = 0$。
代入[@eq:hermitian-def]得
$$
\tr(A H) = \tr(A \cdot \real(H)) .
$$ {\#eq:hermitian-trace-lemma}
[**证毕。**]{.display-block .text-indent-none}

### Ewald summation

本章中的符号与运算含义参见[附录](#附录符号约定)。

通常的Ewald sum由三部分组成：
$$
E = E_r + E_k - E_s ,
$$
其中$E_r$为能量的短程部分，$E_k$为能量的长程部分，$E_s$为自能修正项。
下面分别介绍三个能量项的矩阵求法。

#### 能量短程部分的矩阵计算方法

能量的短程部分$E_r$是一个与点的坐标和电荷相关的量，其定义为
$$
E_r = \frac{1}{2} \sum_{i \neq j}^{\Nr} q_i q_j \cdot \frac{ \erfc(\alpha \|\vec{r_{ij}}\|^2_{\text{L}}) }{ \|\vec{r_{ij}}\|^2_{\text{L}} } .
$$ {\#eq:real-space-def}
其中$q_i$表示第$i$个点的电荷，$r_i$为点的坐标，$\vec{r_{ij}} = \vec{r_i} - \vec{r_j}$为两个点的坐标之差，$\|\vec{r_{ij}}\|^2_{\text{L}}$表示对$\vec{r_{ij}}$使用周期性边界条件化归之后求得的模的平方。

为了使用矩阵加速计算速度，需要将[@eq:real-space-def]中的求和符号转换成矩阵乘法。
构造电荷耦合矩阵$Q$表示每两个电荷的乘积：
$$
Q = \vec{q} \cdot \vec{q}^\T .
$$ {\#eq:q-def}
其中$\vec{q} \in \{0, \pm 1\}^\Nr$表示所有点处的电荷。

矩阵$B_r$表示每两个点之间的相互作用
$$
[B_r]_{ij} = (1 - \delta(i - j)) \frac{ \erfc(\alpha \|\vec{r_{ij}}\|_{\text{L}}^2) }{ \|\vec{r_{ij}}\|_{\text{L}}^2 } .
$$
其中$\delta(i - j)$为狄拉克函数，当且仅当$i = j$时等于1，否则等于0。

能量的短程部分最终可表示为
$$
E_r = \frac{1}{2} \matsum{ B_r \odot Q } .
$$ {\#eq:real-space-calc}

#### 能量长程部分的矩阵计算方法

能量的长程部分$E_k$的计算方法如下：
$$
E_k = \frac{1}{2 L^3} \sum_{\vec{k} \neq 0} v(\vec{k}) \cdot |\hat{\rho}(\vec{k})|^2 ,
$$
其中$\vec{k}$是波矢，$v(\vec{k})$和$\hat{\rho}(\vec{k})$均为关于波矢$\vec{k}$的函数。$v(\vec{k})$和$\hat{\rho}(\vec{k})$的计算公式分别为：
$$
v(\vec{k}) = \frac{4 \pi}{\vec{k} ^ 2} \exp(- \frac{\vec{k}^2}{4 \alpha^2}) ,
$$ {\#eq:v-vec-def}
$$
\hat{\rho}(\vec{k}) = \sum_{j=1}^{N_r} q_j \exp(-i \vec{k}\cdot\vec{r_j}) .
$$ {\#eq:rho-vec-def}
其中$\alpha$是一个参数，$q_i$是第$i$个点处的电荷，$\vec{r_i}$是第$i$个点处的位移。
对于每一个波矢$\vec{k}$，其对应的$v(\vec{k})$和$\hat{\rho}(\vec{k})$都是标量。

为了使用MATLAB快速高效地计算，我们试图将向量运算推广到矩阵维度。
如果用矩阵$K$表示所有的波矢$\vec{k}$，则
$$
K = \vecmat{k}{\Nk} .
$$
矩阵$K \in \matspace{\Nk}{3}$，其中$\Nk$为$\vec{k}$的数量。
此时$\vec{v}$是一个列向量，每一个$v_i$由[@eq:v-vec-def]代入$\vec{k_i}$计算得到。
因此，对于矩阵$K$，有
$$
\vec{v}(K) = 4 \pi \odot \exp(\frac{-1}{4 \alpha^2} \odot |K \otimes K| ) \oslash |K \otimes K| .
$$
其中$\oslash$表示Hadamard积的逆运算，即对待操作的矩阵的每个元素做除法的运算。

与矩阵$K$类似地，我们用矩阵$R$表示所有点的坐标，$R \in \matspace{\Nr}{3}$的矩阵，其中$\Nr$为$\vec{r}$的数量。
此时对于每一个波矢$\vec{k}$有
$$
\hat{\rho}(\vec{k}) = \exp(-i \vec{k} R^\T) \cdot \vec{q}
$$
是一个数量积。
也就是说，每一个$\vec{k}$计算得到的$\hat{\rho}(\vec{k})$是一个数字。
令$\vec{a} = \exp(-i \vec{k} R^\T)$是一个$\Nr$维的行向量，则有
$$
|\hat{\rho}(\vec{k})|^2
= \hat{\rho}(\vec{k}) \cdot \hat{\rho}^\dag (\vec{k})
 = (\vec{a} \vec{q}) (\vec{q}^\T \vec{a}^\dag)
 = \vec{a} Q \vec{a}^\dag .
$$ {\#eq:rho-k-vec}
将[@eq:rho-k-vec]推广到矩阵$K$上有
$$
|\vec{\hat{\rho}}(K)|^2 =
\begin{bmatrix}
  \vec{a_1} Q \vec{a_1}^\dag \\
  \vec{a_2} Q \vec{a_2}^\dag \\
  \vec{a_3} Q \vec{a_3}^\dag \\
  \vdots \\
  \vec{a_{\Nk}} Q \vec{a_{\Nk}}^\dag
\end{bmatrix}
= \diag(A Q A^\dag) ,
$$
其中$\diag(A Q A^\dag)$表示取矩阵对角元素构成的列向量，矩阵$A \in \matspace{\Nk}{\Nr}$满足
$$
A = \vecmat{a}{\Nk} =
\begin{bmatrix}
  \exp(-i \vec{k_1} R^\T) \\
  \exp(-i \vec{k_2} R^\T) \\
  \exp(-i \vec{k_3} R^\T) \\
  \vdots \\
  \exp(-i \vec{k_{\Nk}} R^\T) \\
\end{bmatrix}
= \exp(-i K R^\T) .
$$

于是能量的长程部分可以表示为
$$
E_k = \frac{1}{2 L^3} \vec{v}(K)^\T \cdot \diag(A Q A^\dag) .
$$ {#eq:momentum-space-matrix}
其中$\vec{v}(K)^\T$表示$\vec{v}(K)$的转置，是一个行向量。
为了尽可能将矩阵$Q$分离出来，构造对角矩阵$W = (1 / 2L^3) E \vec{v}$，其中$E$是一个单位矩阵。
可以将[@eq:momentum-space-matrix]变换为
$$
E_k = \tr(W A Q A^\dag) .
$$ {\#eq:momentum-space-last}
其中$\tr$表示对矩阵的所有对角元素求和，也就是矩阵的迹。


#### 自能项的矩阵计算方法

如果用矢量$Q$表示所有的点的电荷，则自能可以表示为$E_s = \alpha / \sqrt{\pi} \cdot \vec{q}^2$。
又因为电荷$q_i \in \{0, \pm 1\}$，所以自能的计算公式为
$$
E_s = \frac{\alpha}{\sqrt{\pi}} \matsum{\abs(\vec{q})} .
$$ {\#eq:self-last}

### 后续用于系统模拟的缓存优化

当空间中的点的坐标不改变时，能量的短程部分$E_k$仅与电荷$Q$有关。
对[@eq:real-space-calc]中的$B_k$稍作变形可以与前面的常数项合并，从而减少浮点数计算次数：
$$
E_r = \frac{1}{2} \matsum{ B_r \odot Q } = \matsum{ (\frac{1}{2} B_r) \odot Q } ,
$$

类似地，能量的长程部分$E_k$也仅与电荷$Q$有关，利用矩阵的迹的性质对[@eq:momentum-space-last]变形，整理得到
$$
E_k = \tr(W A Q A^\dag) = \tr(Q A^\dag W A) .
$$
令$B_k = A^\dag W A$，显然$B_k$是一个厄米矩阵。
[引理1](#lemma-1)已经证过，$\tr(Q B_k) = \tr(Q \cdot \real(B_k))$是一个实数，其中$\real(B_k)$表示取$B_k$的实部。
与此同时，注意到自能项也可以用矩阵的迹表示，于是$E_k$与$E_s$可以合并。
对[@eq:self-last]变形得到
$$
E_s = \tr(Q \cdot \frac{\alpha}{\sqrt{\pi}} I) ,
$$
其中$I \in \matspace{\Nr}{\Nr}$是单位矩阵。

于是系统的总能量可以表示为
$$
\begin{aligned}
E &= E_r + E_k - E_s \\
&= \frac{1}{2} \matsum{ B_r \odot Q } + \tr(Q \cdot \real(B_k)) - \tr(Q \cdot \frac{\alpha}{\sqrt{\pi}} E) \\
&= \matsum{ \frac{1}{2} B_r \odot Q } + \tr\left(Q [\real(B_k) - \frac{\alpha}{\sqrt{\pi}} I]\right) . \\
\end{aligned}
$$
设矩阵$B_r' = (1 / 2) B_r$称为邻接矩阵，矩阵$B_k' = \real(B_k) - (\alpha/ \sqrt{\pi})E$称为系数矩阵，代入上式得
$$
E = \matsum{ Q \odot B_r' } + \tr(Q B_k') .
$$ {\#eq:ewald-summation-to-combine}
其中邻接矩阵$B_r'$满足
$$
[B_r']_{ij} = \sum_{i > j}^{\Nr} \frac{ \erfc(\alpha \cdot \|\vec{r_{ij}}\|_{\text{L}}^2) }{ \|\vec{r_{ij}}\|_{\text{L}}^2 } ,
$$
系数矩阵$B_k'$的计算公式为
$$
\begin{aligned}
B_k' &= \real(B_k) - (\alpha/ \sqrt{\pi})E \\
&= \real(A^\dag W A) - (\alpha/ \sqrt{\pi})E .
\end{aligned}
$$

实验发现，使用MATLAB编写代码计算Ewald sum的时候，计算$E_k$的时间比起计算$E_r$的时间显著地多。
原因是MATLAB在计算$\tr(Q B_k')$的时候，先计算矩阵的乘积然后再求对角的和，时间复杂度自然要多一个数量级（$O(N_r^3)$之于$O(N_r^2)$）。
为了避免不必要的计算，现从矩阵运算的角度优化$\tr(Q B_k')$。
观察发现，
$$
\tr(Q B_k') = \matsum{Q^\T \odot B_k'} = \matsum{Q \odot B_k'} .
$$

于是系统的总能量可以表示为
$$
\begin{aligned}
E &= \matsum{Q \odot D},& \\
D &= \frac{1}{2} B_r + \real(B_k) - \frac{\alpha}{\sqrt{\pi}} I. &
\end{aligned}
$$ {\#eq:ewald-sum-last}
其中矩阵$D$表征系统的除电荷以外的所有信息，不妨称为Ewald矩阵。
实际上，Ewald sum可以定义为$E: \{0, \pm 1\}^\Nr \to \mathbb{R}$是一个二次型，即
$$
E(\vec{q}) = \matsum{Q \odot D} = \tr(Q D) = \tr(\vec{q}^\T D \vec{q}) = \vec{q}^\T D \vec{q} .
$$ {\#eq:ewald-sum-quadratic}

### 结论

对RPM系统进行模拟时，实际上只需要改变各个点处的电荷，而Ewald矩阵$D$不需要改变。
因此，如果预先将Ewald矩阵预先计算出来，则每次模拟时系统的总能量的求解等同于[@eq:ewald-sum-quadratic]的值的求解。
该式的时间复杂度为$O(\Nr^2)$，浮点数运算次数（包括乘法和加法）为$0.5 \Nr^2 + 3.5 \Nr - 1$ (GFlops)。

分别使用MATLAB和C++编程（调用OpenBLAS库进行矩阵计算）对$L = 12$的三维空间内有72对正负带电粒子的系统进行模拟。
计算所使用的服务器有40个CPU，型号为Intel(R) Xeon(R) Silver 4316 CPU \@ 2.30GHz。
MATLAB计算速度约为$1.3 \times 10^{-4}$ s，C++的计算速度约为$1.1 \times 10^{-6}$ s。

结果表明，将公式里的向量运算和求和改写成矩阵运算，不仅可以有效地减少重复计算，而且还能将电荷与其他矩阵分离，从而可以预先计算Ewald矩阵$D$，进一步加快运算速度，并且使GPU加速变为可能。

<!-- Finish -->
[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}

### 附录：符号约定 {data-toc-unnumbered=true}

  符号/运算                  含义
------------          ---------------
$E$                   总能量
$E_r$                 能量的短程部分
$E_k$                 能量的长程部分
$E_s$                 自能修正
$L$                   边界条件内某一维度的大小
$\alpha$              反正是个参数
$\Nr$                 总粒子数
$\Nk$                 总波矢数
$\Nq$                 带电粒子个数
$\vec{r_i}$           某特定粒子的坐标，矢量
$\vec{k_i}$           动量空间里的一个波矢，矢量
$\vec{q}$             所有粒子电荷，矢量
$R$                   表示所有粒子坐标的矩阵
$K$                   表示所有波矢的矩阵
$Q$                   描述每两个电荷之间的耦合的矩阵
$\vec{k}\cdot\vec{r}$ 向量$\vec{k}$与向量$\vec{r}$的数量积
$I$                   尺寸灵活的单位矩阵
$|A|^2$               复矩阵$A$的模
$A^\T$                矩阵$A$的转置
$A^\dag$              复矩阵$A$的共轭转置
$A \cdot B$           矩阵$A$与$B$的乘积
$A \odot B$           矩阵$A$与$B$的Hadamard积
$A \oslash B$         矩阵$A$与$B$的Hadamard积的逆运算
$|A \otimes A|$       矩阵$A$的[数值平方](#矩阵的数值平方)

: [Ewald summation章节](#ewald-summation)里用到的符号及其含义表 {#tbl:symbols}
