---
title: Weekend 08-03
tags: Life
date: 2026-03-08 15:15:51
katex: true
---


今日是开学后的第一个周末，这学期是史上最短的一学期。

好久没打 Atcoder 了。这周 Atcoder D 题水成啥了，一个 DFS 就结束了。（第一次被卡 hash 了

# 关于 E 题

[原题](https://atcoder.jp/contests/abc448/tasks/abc448_e)

## 题意

给定一个不含前导 $0$ 的大整数 $N$ 由 $K$ 段组成，第 $i$ 段由 $l_i$ 个 $c_i$ 组成。（$1\le K \le 10^5,1\le l_i \le 10^9,0\le c_i\le 9$）

给定一个正整数 $M$（$1\le M \le 10^4$），求 $\left\lfloor\frac{N}{M}\right\rfloor\pmod{1007}$

## 小定理

$$
\left\lfloor\frac{a}{b}\right\rfloor\pmod{c}=\left\lfloor\frac{a\pmod{bc}}{b}\right\rfloor
$$

<details>
<summary>理解：</summary>

数轴上从 $0$ 开始每隔 $b - 1$ 个数划为一个小区间，形成若干段长度为 $b$ 的小区间，从左到右依次编号为 $0,1,2,\dots$。这里每个数的编号就代表着 $\left\lfloor\frac{a}{b}\right\rfloor$ 的值。

然后类似的，每隔 $c - 1$ 个区间划为一个大区间，形成若干段长度为 $c$ 的大区间，对于每个大区间内部从左到右依次编号 $0,1,2,\dots,c-1$。这里每个小区间所对应的编号就代表着 $\left\lfloor\frac{a}{b}\right\rfloor\pmod{c}$ 的值。

不难看出，我们可以颠倒两步的顺序：

1. 从 $0$ 开始每隔 $bc - 1$ 划分区间，再在每个区间内部编号。
2. 再在每隔区间内部每隔 $b - 1$ 划分区间，再对每个区间编号。

而这对应着先取模再除。

也就是说先除再取模和先取模再除是等价的。

</details>

## Solution 1

首先，由上述定理，原文题等价于求 $\left\lfloor\frac{N \pmod{M \times 10007}}{M}\right\rfloor$，对于每一个循环的部分考虑倍增，对于长度为 $2^n$ 次方的 $111\dots1$ 进行求解。对于询问，将 $l_i$ 进行二进制拆分，累计贡献即可。

## Solution 2

我们发现 $999\dots9=10^n-1$ 是 $111\dots1$ 的 $9$ 倍，并且可以用快速幂求。

那么我们为什么不进一步使用定理，得到 $\left\lfloor\frac{9N \pmod{M \times 10007 \times 9}}{9M}\right\rfloor$

举个例子，当 $l_i=5,c_i=3$ 时，$9N=33333=3\times99999=3\times(10^5-1)$。快速幂计算即可。
