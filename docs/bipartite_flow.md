---
title: Bipartite Flow
documentation_of: ../src/algorithm/matching/bipartite_flow.hpp
---

二部グラフのフロー。

`source -> left -> right -> sink` の形のネットワークを作って最大流を流す。

# コンストラクタ

```cpp
BipartiteFlow<T> graph(left_size, right_size)
```

# 関数

```cpp
graph.add_left_capacity(left, cap)
graph.add_right_capacity(right, cap)
graph.add_edge(left, right, cap)
graph.max_flow()
```

`max_flow()` は `BipartiteFlowResult<T>` を返す。

```cpp
T flow;
vector<DinicEdge<T>> edges;
```

## 時間計算量

Dinic法に依存する。
