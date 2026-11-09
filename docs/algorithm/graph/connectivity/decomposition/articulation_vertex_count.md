---
title: Articulation Vertex Count (関節点数) [SUBMERGE]
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/articulation_vertex_count.hpp
---

無向グラフの関節点の個数を、Lowlink 法（`lowlink`）で求める。

## articulation_vertex_count

```cpp
int articulation_vertex_count(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

## 引数

- `vertex_count`: 頂点数 $N$。
- `edges`: 0-indexed の端点を持つ無向辺の列。

## 戻り値

削除すると連結成分数が増える頂点（関節点）の個数。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$、関節点数を $A$、橋の本数を $B$ とする。内部で `lowlink` を呼び、関節点と橋の列をソートする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `articulation_vertex_count` | $O(N + M + A \log (A + 1) + B \log (B + 1))$ | $O(N + M)$ |

## 注意点

- `vertex_count` は非負、端点は $[0, N)$ でなければならない。違反時は `std::runtime_error` を送出する。
- 多重辺はそれぞれ別の辺として扱う。
- DFS は再帰で実装されており、再帰の深さは最大 $N$ になる。
