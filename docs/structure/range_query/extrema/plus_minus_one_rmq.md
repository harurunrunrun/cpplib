---
title: ±1 RMQ (±1区間最小値)
documentation_of: ../../../../src/structure/range_query/extrema/plus_minus_one_rmq.hpp
---

隣接する要素の差が常に $+1$ または $-1$ の整数列に特化した静的な RMQ である。

## API

- `explicit PlusMinusOneRMQ(vector<int> values)`: 列を前処理する。
- `size()`: 列の長さを返す。
- `argmin(left, right)`: 半開区間 `[left, right)` の最小値の添字のうち、最も左のものを返す。
- `minimum(left, right)`: 最小値を返す。

## 時間計算量

列の長さを $N$ とする。

- 構築: $O(N\log N)$（同じ型のブロックの表は共有する）
- `size`, `argmin`, `minimum`: $O(1)$

## 注意点

- 隣接する差がすべて厳密に `1` または `-1` でなければ、コンストラクタが `std::invalid_argument` を送出する。
- クエリ区間は空でない半開区間とする。`0 <= left < right <= size()` を満たさなければ `std::out_of_range` を送出する。
