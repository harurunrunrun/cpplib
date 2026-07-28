---
title: 2D Sparse Table (二次元スパーステーブル)
documentation_of: ../../../../src/structure/range_query/aggregation/sparse_table_2d.hpp
---

静的行列の冪等・結合的演算を長方形上で取得する。

## API

- `SparseTable2D(matrix,operation)`: 行列を前処理する。
- `height()`, `width()`: 行数、列数を返す。
- `prod(top,left,bottom,right)`: 半開長方形の演算結果を返す。

## 時間計算量

- 構築: $O(HW\log H\log W)$。
- `prod`, `height`, `width`: $O(1)$。

## 注意点

- `operation` は結合的かつ冪等でなければならない。空長方形は扱わない。
