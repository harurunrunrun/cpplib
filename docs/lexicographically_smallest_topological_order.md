---
title: Lexicographically Smallest Topological Order (辞書順最小トポロジカル順序) [TOPOSORT]
documentation_of: ../src/algorithm/graph/lexicographically_smallest_topological_order.hpp
---

# lexicographically_smallest_topological_order

```cpp
optional<vector<int>> lexicographically_smallest_topological_order(
    const vector<vector<int>>& graph
)
```

## 引数

- `graph`: 0-indexed頂点の有向隣接リスト。

## 戻り値

辞書順最小のトポロジカル順序。cycleがあれば `nullopt`。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `lexicographically_smallest_topological_order` | $O((N+M)\log(N+1))$ | $O(N)$ |

## 注意点

- 各辺の行き先は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。
