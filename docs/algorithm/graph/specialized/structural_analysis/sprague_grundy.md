---
title: Sprague-Grundy Numbers on DAG (DAG上のスプレイグ・グランディ数)
documentation_of: ../../../../../src/algorithm/graph/specialized/structural_analysis/sprague_grundy.hpp
---

有限かつ非巡回な不偏ゲームを有向グラフで表し、各状態の Grundy 数を、遷移先の Grundy 数の mex として求める。

## `dag_grundy_numbers`

```cpp
vector<size_t> dag_grundy_numbers(const vector<vector<size_t>>& graph)
```

`graph[v]` を状態 `v` から 1 手で移れる状態の列として、全状態の Grundy 数を返す。

## `normal_play_nim_sum`

```cpp
size_t normal_play_nim_sum(
    const vector<size_t>& grundy,
    const vector<size_t>& positions
)
```

独立な複数の局面 `positions` について、それぞれの Grundy 数 `grundy[p]` の XOR を返す。通常ルール（最後に動けた方が勝ち）のもとで、戻り値が $0$ なら後手必勝、$0$ でなければ先手必勝である。

## 時間計算量

頂点数を $V$、辺数を $E$、局面数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `dag_grundy_numbers` | $O(V+E)$ | $O(V)$ |
| `normal_play_nim_sum` | $O(K)$ | $O(1)$ |

## 注意点

- `dag_grundy_numbers` は、グラフに閉路（自己ループを含む）があれば `invalid_argument`、範囲外の頂点番号があれば `out_of_range` を送出する。
- `normal_play_nim_sum` は、`positions` に `grundy.size()` 以上の値があれば `out_of_range` を送出する。
- 有限 DAG 上の通常ルールの不偏ゲームを前提とする。
