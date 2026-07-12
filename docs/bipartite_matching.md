---
title: Bipartite Matching
documentation_of: ../src/algorithm/matching/bipartite_matching.hpp
---

二部グラフの最大マッチング。

# コンストラクタ

```cpp
BipartiteMatching graph(left_size, right_size)
```

# 関数

```cpp
graph.add_edge(left, right)
graph.solve()
```

`BipartiteMatchingResult` は次を持つ。

```cpp
int size;
vector<int> left_match;
vector<int> right_match;
```

## 時間計算量

- $O(E\sqrt V)$
