---
title: Rearrange Rows into Permutation Columns (行内並べ替えによる列置換化) [ABC317_G]
documentation_of: ../src/algorithm/graph/rearrange_rows_into_permutation_columns.hpp
---

各行の要素を行内だけで並べ替え、すべての列を `0,1,...,N-1` の置換にする。

## `rearrange_rows_into_permutation_columns`

```cpp
vector<vector<int>> rearrange_rows_into_permutation_columns(
    const vector<vector<int>>& matrix
)
```

`N` 行 `M` 列の `matrix` と同じ形の行列を返す。戻り値の各行は対応する入力行と
同じ多重集合を持ち、各列には `0` 以上 `N` 未満の値が1回ずつ現れる。
空行列には空行列を返し、`M == 0` も扱える。

## 時間計算量

`E = NM` とすると
$O(E\sqrt{N+1}\log(M+1)+N+E)$。

## 空間計算量

戻り値を含めて $O(N+E)$。

## 注意点

- 行列は長方形でなければならない。
- 各要素は `0` 以上 `N` 未満で、各値は行列全体にちょうど `M` 回現れなければ
  ならない。
- 要素数は `int` で表現可能でなければならない。
- 前提違反では `runtime_error` を送出する。
