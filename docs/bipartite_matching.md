---
title: Bipartite Matching (二部マッチング) [bipartitematching] [GRL_7_A]
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

## 注意点

match列は対応端点または `-1`。public `bfs()` はlayerを作って増加路候補の有無を返し、`dfs(left)` はそのlayerを使って1回増加する実装helper。通常は全辺追加後に `solve()` を1回呼ぶ。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error`。容量・cost・総和の演算結果が `T` に収まることを前提とする。記載した計算量には引数検査とResultの構築を含む。
