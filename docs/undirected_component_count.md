---
title: Undirected Component Count (無向グラフの連結成分数) [CAM5]
documentation_of: ../src/algorithm/graph/connectivity/undirected_component_count.hpp
---

無向グラフの連結成分数を求める。

## undirected_component_count

```cpp
int undirected_component_count(
    int vertex_count,
    const vector<pair<int, int>>& edges
);
```

## 引数

- `vertex_count`: 頂点数。
- `edges`: 0-indexedの端点を持つ無向辺列。

## 戻り値

孤立頂点を含む連結成分数。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$、逆Ackermann関数を $\alpha$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `undirected_component_count` | $O((N+M)\alpha(N))$ | $O(N)$ |

## 注意点

- `vertex_count` は非負、各端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。
- 自己ループと多重辺を許す。
