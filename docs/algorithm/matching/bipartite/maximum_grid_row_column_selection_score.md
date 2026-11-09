---
title: Maximum Grid Row and Column Selection Score (格子の行・列選択最大得点) [ABC259_G]
documentation_of: ../../../../src/algorithm/matching/bipartite/maximum_grid_row_column_selection_score.hpp
---

符号付きの重み行列について、行と列の選び方に制約がある場合の最大得点を返す。

## maximum_grid_row_column_selection_score

```cpp
long long maximum_grid_row_column_selection_score(
    const vector<vector<long long>>& grid
)
```

任意の行の集合と列の集合を選ぶ。ただし、負の値を持つマスについては、その行と列を同時に選ぶことはできない。選んだ行または選んだ列に属する各マスの値を 1 回ずつ足し合わせた得点の最大値を返す。空の行列または幅 $0$ の行列には $0$ を返す。

内部では行・列を変数とする燃やす埋める問題（`BurnBury`）に帰着し、最小カットで解く。

## 時間計算量

高さを $H$、幅を $W$ とする。現在の Dinic 実装の上界により、$O(HW + (H+W)^2 HW)$。

## 空間計算量

$O(HW + H + W)$。

## 注意点

- 行列は長方形でなければならない。違反時は `runtime_error` を送出する。$H$, $W$, $H + W$ が `int` に収まらない場合も `runtime_error` を送出する。
- 絶対値の総和から有限の禁止コストを構成する。絶対値の総和が $2^{62} - 1$ を超え、`long long` で安全に扱えない場合は `overflow_error` を送出する。
- 行も列も選ばない選択が許されるため、戻り値は常に $0$ 以上である。
