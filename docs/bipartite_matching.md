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

$V=L+R$、$E$ を追加済み辺数とする。

- `BipartiteMatching(L, R)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- `bfs`: $O(V+E)$
- `dfs(v)`: 1回 worst-case $O(V+E)$
- `solve`: Hopcroft--Karp法で $O(E\sqrt V+V)$。長さ $L,R$ の対応列の出力を含む

## 空間計算量

- graph、matching、戻り値を含めて $O(V+E)$
