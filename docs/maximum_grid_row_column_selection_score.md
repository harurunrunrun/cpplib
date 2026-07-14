---
title: Maximum Grid Row and Column Selection Score (格子の行・列選択最大得点) [ABC259_G]
documentation_of: ../src/algorithm/matching/maximum_grid_row_column_selection_score.hpp
---

符号付き重み行列に対する、行と列の選択制約付き最大得点を返す。

# maximum_grid_row_column_selection_score

```cpp
long long maximum_grid_row_column_selection_score(
    const vector<vector<long long>>& grid
)
```

任意の行集合と列集合を選ぶ。負の要素があるマスでは、その行と列を同時に選べない。
選んだ行または選んだ列に属する各マスの値を一度ずつ加えた得点の最大値を返す。
空の行列または幅 $0$ の行列には $0$ を返す。

## 時間計算量

高さを $H$、幅を $W$ とする。現在のDinic実装の上界により、
$O(HW+(H+W)^2HW)$。

## 空間計算量

$O(HW+H+W)$。

## 注意点

- 行列は長方形でなければならない。違反時は `runtime_error` を送出する。
- 絶対値の総和から有限の禁止コストを構成する。安全に `long long` で扱えない場合は
  `overflow_error` を送出する。
- 戻り値は常に $0$ 以上である。行も列も選ばない選択が許される。
