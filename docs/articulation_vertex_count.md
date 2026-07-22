---
title: Articulation Vertex Count (関節点数) [SUBMERGE]
documentation_of: ../src/algorithm/graph/connectivity/articulation_vertex_count.hpp
---

## articulation_vertex_count

```cpp
int articulation_vertex_count(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

## 引数

- `vertex_count`: 頂点数。
- `edges`: 0-indexed端点を持つ無向辺列。

## 戻り値

削除すると連結成分数が増える頂点の個数。

## API別の時間計算量・空間計算量

$N=vertex_count$、$M=edges.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `articulation_vertex_count` | $O(N+M)$ | $O(N+M)$ |

## 注意点

- `vertex_count` は非負、端点は $[0,N)$ でなければならない。
- 前提違反時は `runtime_error` を送出する。並列辺は別の辺として扱う。
