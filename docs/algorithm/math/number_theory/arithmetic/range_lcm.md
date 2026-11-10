---
title: Range Least Common Multiple (列全体の最小公倍数) [NTL_1_C]
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/range_lcm.hpp
---

## API

- `math::range_lcm(values)`: 整数を要素とする range `values` 全体の最小公倍数を、非負の `long long` で返す。各要素は `long long` に変換してから `math::lcm` で順に畳み込む。

## 時間計算量・空間計算量

要素数を $N$、要素の絶対値の最大値を $U$ として、時間 $O(N \log(U+1))$、追加領域 $O(1)$。

## 注意点

- 空の range では単位元 $1$ を返し、$0$ を含む range では $0$ を返す。
- 各要素の絶対値と途中結果は `long long` で表現できなければならない。最小公倍数の計算でのオーバーフローは検査しない。
