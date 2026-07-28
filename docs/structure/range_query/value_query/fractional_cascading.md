---
title: Fractional Cascading (フラクショナルカスケーディング)
documentation_of: ../../../../src/structure/range_query/value_query/fractional_cascading.hpp
---

複数の整列済み列に対する同じ値の二分探索を、一度の二分探索と層間リンクで処理する。

## API

- `FractionalCascading(catalogs)`: 整列済み列群を前処理する。
- `catalog_count()`: 列数を返す。
- `lower_bounds(value)`: 各列の `lower_bound` 添字を返す。

## 時間計算量

- 構築: $O(S\log S)$。
- `lower_bounds`: $O(\log S+K)$。
- `catalog_count`: $O(1)$。

## 注意点

- $S$ は全要素数、$K$ は列数。各入力列は昇順でなければならない。
