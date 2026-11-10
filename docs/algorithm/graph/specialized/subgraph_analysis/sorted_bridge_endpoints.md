---
title: Sorted Bridge Endpoints (ソート済み橋端点列挙) [EC_P]
documentation_of: ../../../../../src/algorithm/graph/specialized/subgraph_analysis/sorted_bridge_endpoints.hpp
---

無向グラフの橋を端点の組として正規化し、辞書順に列挙する。

## `sorted_bridge_endpoints`

```cpp
vector<pair<int, int>> sorted_bridge_endpoints(
    int vertex_count,
    const vector<pair<int, int>>& edges
);
```

### 引数

- `vertex_count`: 頂点数
- `edges`: 0-indexed の端点を持つ無向辺の列

### 戻り値

各橋を小さい端点が先になる組 `(u, v)`（`u <= v`）として、辞書順に並べて返す。

## API別の時間計算量・空間計算量

$N$ を `vertex_count`、$M$ を `edges.size()`、橋の本数を $B$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `sorted_bridge_endpoints` | $O(N+M+B\log(B+1))$ | $O(N+M)$ |

## 注意点

- `vertex_count` は非負、端点は $[0,N)$ でなければならない。
- 自己ループと多重辺を許し、多重辺は別々の辺として扱う。
- 前提違反時は `runtime_error` を送出する。
