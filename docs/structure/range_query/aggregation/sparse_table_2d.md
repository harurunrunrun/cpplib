---
title: 2D Sparse Table (二次元スパーステーブル)
documentation_of: ../../../../src/structure/range_query/aggregation/sparse_table_2d.hpp
---

静的な行列に対して、冪等かつ結合的な演算の長方形領域での積を求める。

## API

```cpp
template<class T, class Operation>
class SparseTable2D;
```

- `SparseTable2D()`: 空のテーブルを構築する。
- `SparseTable2D(const vector<vector<T>>& matrix, Operation operation = Operation{})`: 行列を前処理する。
- `height()`, `width()`: 行数、列数を返す。
- `prod(top, left, bottom, right)`: 半開の長方形 $[\mathit{top}, \mathit{bottom}) \times [\mathit{left}, \mathit{right})$ の演算結果を返す。

## 時間計算量

行列の大きさを $H \times W$ とする。

- 構築: $O(HW\log H\log W)$
- `prod`, `height`, `width`: $O(1)$

空間計算量は $O(HW\log H\log W)$。

## 注意点

- `operation` は結合的かつ冪等でなければならない。
- 行ごとに長さが異なる行列を渡すと `std::invalid_argument` を送出する。
- 空の長方形は扱わない。`prod` の引数が `0 <= top < bottom <= H` かつ `0 <= left < right <= W` を満たさなければ `std::out_of_range` を送出する。
