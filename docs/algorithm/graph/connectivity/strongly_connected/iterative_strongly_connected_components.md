---
title: Iterative Strongly Connected Components (反復強連結成分分解)
documentation_of: ../../../../../src/algorithm/graph/connectivity/strongly_connected/iterative_strongly_connected_components.hpp
---

有向グラフの強連結成分分解を、再帰を使わない Kosaraju 法で行う。深いグラフでもコールスタックを消費しない。

## iterative_strongly_connected_components

```cpp
StronglyConnectedComponentsResult iterative_strongly_connected_components(
    const vector<vector<int>>& graph
)
```

## 引数

- `graph`: 0-indexed の頂点からなる有向グラフの隣接リスト。

## 戻り値

`count` に成分数、`id[v]` に頂点 `v` の成分番号、`groups[c]` に成分 `c` の頂点列を持つ `StronglyConnectedComponentsResult`。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `iterative_strongly_connected_components` | $O(N + M)$ | $O(N + M)$ |

## 注意点

- 各辺の行き先は $[0, N)$ でなければならない。違反時は `std::out_of_range` を送出する。頂点数が `int` の最大値を超える場合は `std::length_error` を送出する。
- 成分番号の具体的な値に依存しないこと。
