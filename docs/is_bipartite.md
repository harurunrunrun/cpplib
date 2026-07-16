---
title: Bipartite Graph Test (二部グラフ判定) [BUGLIFE]
documentation_of: ../src/algorithm/graph/is_bipartite.hpp
---

無向グラフを2色で矛盾なく彩色できるかを判定する。

## is_bipartite

```cpp
bool is_bipartite(
    int vertex_count,
    const vector<pair<int, int>>& edges
);
```

頂点集合を、各辺の両端が異なる集合に属するよう2分割できる場合に `true` を返す。非連結グラフにも対応する。

## 引数

- `vertex_count`: 頂点数。
- `edges`: 0-indexedの端点を持つ無向辺列。

## 戻り値

グラフが二部グラフなら `true`、そうでなければ `false`。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `is_bipartite` | $O(N+M)$ | $O(N+M)$ |

## 注意点

- `vertex_count` は非負、各端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。
- 自己ループを含むグラフでは `false` を返す。
- 多重辺を許す。
