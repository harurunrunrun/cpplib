---
title: Rearrange Rows into Permutation Columns (行内並べ替えによる列置換化) [ABC317_G]
documentation_of: ../../../../../src/algorithm/graph/specialized/construction/rearrange_rows_into_permutation_columns.hpp
---

各行の要素をその行の中だけで並べ替え、すべての列を $0, 1, \ldots, N-1$ の置換にする。二部グラフの辺彩色に帰着して求める。

## `rearrange_rows_into_permutation_columns`

```cpp
std::vector<std::vector<int>> rearrange_rows_into_permutation_columns(
    const std::vector<std::vector<int>>& matrix
)
```

$N$ 行 $M$ 列の `matrix` と同じ形の行列を返す。戻り値の各行は対応する入力の行と同じ多重集合をもち、各列には $0$ 以上 $N$ 未満の値が 1 回ずつ現れる。空の行列には空の行列を返し、$M = 0$ も扱える。

## 時間計算量

$E = NM$ とすると $O(E\sqrt{N + 1}\log(M + 1) + N + E)$。

## 空間計算量

戻り値を含めて $O(N + E)$。

## 注意点

- 行列は長方形でなければならない。
- 各要素は $0$ 以上 $N$ 未満で、各値は行列全体にちょうど $M$ 回現れなければならない。
- 要素数は `int` で表現できなければならない。
- これらの前提に違反すると `runtime_error` を送出する。
