---
title: Binary Expansion Operations (浮動小数点展開の二項演算)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/mixed/geometry3d_expansion_binary.hpp
---

2 個の浮動小数点展開（`Geometry3DExpansion`）に対する和と積を提供する。

## API

- `geometry3d_expansion_sum(first, second)`: 2 つの展開の厳密な和を表す展開を返す。
- `geometry3d_expansion_product(first, second)`: 2 つの展開の厳密な積を表す展開を返す。

## API別の時間計算量・空間計算量

`first`, `second` の成分数をそれぞれ $N, M$ とする。

- `geometry3d_expansion_sum`: `second` の各成分を 1 つずつ加える（grow-expansion）ため、時間 $O(M(N+M))$。
- `geometry3d_expansion_product`: `first` を `second` の各成分倍した展開を順に足し合わせるため、最悪時間 $O(N^2 M^2)$。

戻り値と中間展開の空間は、生成される成分数に比例する。

## 注意点

- 入力の成分は絶対値の小さい順に並べておくこと。
- 非有限な成分を含む入力には `std::invalid_argument` を送出する。演算結果がオーバーフローした場合は `std::overflow_error` を送出する。
- 空の展開は $0$ として扱う。結果は少なくとも 1 成分（$0$ のみの場合は `{0}`）を持つ。
