---
disableNunjucks: true
mathjax: false
title: 符号回归与LaTeX公式
tags:
  - Python
  - 机器学习
  - LaTeX
categories:
  - 计算机与通信工程
description: >-
  这篇文章介绍了如何将gplearn生成的公式转换为人类可读格式，主要包括使用SymPy将公式转换为LaTeX表达式以及通过在线API将LaTeX公式转为SVG图片。文章还列举了所需的Python库，并提供了具体实现流程和结果展示。整体流程分为从gplearn到SymPy、再到LaTeX，最后生成SVG图片。
date: 2022-01-10 13:56:52
---

> [前文](/2022/01/01/符号回归与画图/)讲到，gplearn支持导出dot语言脚本，进而可以利用graphviz画图。
本文将要尝试实现，将gplearn生成的公式转换成人类可读格式。

<!-- more -->

### 公式转换流程

![公式转换流程图](/notes/images/graphviz/formula_convert.svg){#fig:formula-convert}

本文所介绍的公式转换流程如[@fig:formula-convert]所示。

### 需要下载安装的Python库

- requests
- sympy

### SymPy

[18年的stackoverflow回答](https://stackoverflow.com/questions/48404263/how-to-export-the-output-of-gplearn-as-a-sympy-expression-or-some-other-readable)给出了一种解决方案：使用SymPy进行公式转换。

```py
converter = {
    'sub': lambda x, y : x - y,
    'div': lambda x, y : x/y,
    'mul': lambda x, y : x*y,
    'add': lambda x, y : x + y,
    'neg': lambda x    : -x,
    'pow': lambda x, y : x**y
}

sympy.sympify('sqrt(div(add(1.000, X0), mul(-0.993, X0)))', locals=converter)
```

上述代码将公式转换成sympy的内部格式，然后使用`sympy.latex`就可以输出LaTeX公式了。

### LaTeX公式在线转换API

LaTeX公式也不是非常直观，为了将其转换为SVG格式图片，我写出并部署了一个基于MathJax的在线转换API。
URL为`https://mathjax.cerallin.top/?latex=E%3Dmc^2`。

### 结合gplearn使用

按照[@fig:formula-convert]所示流程，代码可以分成三部分。

1. gplearn -> SymPy

```py
from sympy import sqrt, log, abs, max, min, sin, cos, tan

# 转换成人类可读的公式
converter = {
    'sub':  lambda x, y: x - y,
    'div':  lambda x, y: x / y,
    'mul':  lambda x, y: x * y,
    'add':  lambda x, y: x + y,
    'sqrt': lambda x   : sqrt(x),
    'log':  lambda x   : log(x),
    'abs':  lambda x   : abs(x),
    'neg':  lambda x   : -x,
    'inv':  lambda x   : 1 / x,
    'max':  lambda x, y: max(x, y),
    'min':  lambda x, y: min(x, y),
    'sin':  lambda x   : sin(x),
    'cos':  lambda x   : cos(x),
    'tan':  lambda x   : tan(x),
}

formula = sympy.sympify(str(est_gp._program), locals=converter)
```

2. SymPy -> LaTeX

```py
# 转换成LaTeX公式
formula_latex = sympy.latex(formula)
```

3. LaTeX -> SVG

```py
# 根据LaTeX公式转换成SVG图片
import requests

svg_res = requests.get('https://mathjax.cerallin.top/', {
    'latex': formula_latex
}).text

# 输出SVG图片到文件
with open('SymbolicRegressor'+'.svg', "w", encoding='utf-8') as f:
    f.write("%s" % svg_res)
    f.close()
```

### 结果展示

#### gplearn输出结果

![gplearn预测的符号树](/notes/images/graphviz/gplearn_result.svg)

#### SymPy公式

```py
X3 + (X2 + X5)*(X0*X1*X5 + X1) + 43.151
```

#### LaTeX公式

```latex
$$
X_{3} + \left(X_{2} + X_{5}\right) \left(X_{0} X_{1} X_{5} + X_{1}\right) + 43.151
$$
```

#### SVG图片

![根据LaTeX公式生成的SVG图片](/notes/images/SymbolicRegressor.svg)

[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
