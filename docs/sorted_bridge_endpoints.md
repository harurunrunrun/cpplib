---
title: Sorted Bridge Endpoints (ソート済み橋端点列挙) [EC_P]
documentation_of: ../src/algorithm/graph/specialized/sorted_bridge_endpoints.hpp
---

無向graphの橋を端点pairとして正規化し、辞書順に列挙する。

## `sorted_bridge_endpoints`

```cpp
vector<pair<int, int>> sorted_bridge_endpoints(
    int vertex_count,
    const vector<pair<int, int>>& edges
);
```

## 引数

- `vertex_count`: 頂点数。
- `edges`: 0-indexed端点を持つ無向辺列。

## 戻り値

橋を、小さい端点を先にしたpairの辞書順で返す。

## API別の時間計算量・空間計算量

$N=vertex_count$、$M=edges.size()$、切断辺数を $B$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `sorted_bridge_endpoints` | $O(N+M+B\log(B+1))$ | $O(N+M)$ |

## 注意点

- `vertex_count` は非負、端点は $[0,N)$ でなければならない。
- 自己loopと並列辺を許し、並列辺は別の辺として扱う。
- 前提違反時は `runtime_error` を送出する。
