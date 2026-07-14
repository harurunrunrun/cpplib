---
title: Range Least Common Multiple (列全体の最小公倍数) [NTL_1_C]
documentation_of: ../src/algorithm/math/range_lcm.hpp
---

## API

- <code>math::range_lcm(values)</code>: 整数range <code>values</code> 全体の非負な最小公倍数を返す。

## API別の時間計算量・空間計算量

- 要素数を $N$、要素の絶対値の最大値を $U$ として、時間 $O(N\log(U+1))$、追加領域 $O(1)$。

## 注意点

空rangeでは単位元1を返し、0を含むrangeでは0を返す。各要素の絶対値と途中結果は <code>long long</code> で表現できなければならない。最小公倍数の乗算overflowは検査しない。
