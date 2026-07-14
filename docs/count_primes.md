---
title: Count Primes (素数個数) [ALDS1_1_C]
documentation_of: ../src/algorithm/math/count_primes.hpp
---

## API

- <code>math::count_primes(values)</code>: 整数range <code>values</code> に含まれる素数の個数を返す。

## API別の時間計算量・空間計算量

- 要素数を $N$、要素の絶対値の最大値を $U$ として、時間 $O(N\log(U+1))$、追加領域 $O(1)$。

## 注意点

空rangeでは0を返す。符号付き整数の負数、0、1は素数として数えない。各非負要素を64 bit符号なし整数へ変換し、決定的Miller-Rabinで判定する。
