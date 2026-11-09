---
title: Lexicographically Smallest Topological Order (辞書順最小トポロジカル順序) [TOPOSORT]
documentation_of: ../../../../src/algorithm/graph/traversal/lexicographically_smallest_topological_order.hpp
---

## `lexicographically_smallest_topological_order`

```cpp
optional<vector<int>> lexicographically_smallest_topological_order(
    const vector<vector<int>>& graph
)
```

### 引数

- `graph`: 頂点番号が 0-indexed の有向グラフの隣接リスト

### 戻り値

辞書順最小のトポロジカル順序を返す。閉路があれば `nullopt` を返す。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `lexicographically_smallest_topological_order` | $O(N\log(N+1)+M)$ | $O(N)$ |

## 注意点

- 各辺の行き先は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。
- 頂点数が `int` に収まらない場合は `length_error` を送出する。
