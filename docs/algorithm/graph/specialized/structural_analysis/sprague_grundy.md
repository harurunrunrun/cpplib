---
title: Sprague-Grundy Numbers on DAG (DAG上のスプレイグ・グランディ数)
documentation_of: ../../../../../src/algorithm/graph/specialized/structural_analysis/sprague_grundy.hpp
---

有限非巡回な不偏ゲームを有向グラフで表し、各状態の Grundy 数を遷移先 Grundy 数の MEX として求める。

## dag_grundy_numbers

```cpp
vector<size_t> dag_grundy_numbers(const vector<vector<size_t>>& graph)
```

`graph[v]` を状態 `v` から 1 手で移れる状態列として、全状態の Grundy 数を返す。

## normal_play_nim_sum

```cpp
size_t normal_play_nim_sum(
    const vector<size_t>& grundy,
    const vector<size_t>& positions
)
```

独立な複数局面の Grundy 数を XOR する。戻り値が 0 なら通常手番規則で後手必勝、非 0 なら先手必勝である。

## 時間計算量

頂点数を $V$、辺数を $E$、局面数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `dag_grundy_numbers` | $O(V+E)$ | $O(V)$ |
| `normal_play_nim_sum` | $O(K)$ | $O(1)$ |

## 注意点

自己 loop を含む cycle があれば `invalid_argument`、範囲外の頂点番号があれば `out_of_range` を送出する。有限 DAG 上の通常手番・不偏ゲームを前提とする。
