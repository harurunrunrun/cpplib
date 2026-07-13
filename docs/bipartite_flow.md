---
title: Bipartite Flow (二部グラフフロー)
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

$L,R$ を左右の頂点数、$E$ を左右間に追加した辺数とし、構築するnetworkを $V'=L+R+2$, $E'=L+R+E$ とする。

- `BipartiteFlow(L, R)`: $O(L+R)$
- `add_left_capacity`, `add_right_capacity`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `max_flow`: network構築・辺出力 $O(V'+E')$ とDinic法 $O(V'^2E')$

## 空間計算量

- 保存中の入力と一時Dinic graph、戻り値を含めて $O(V'+E')$

## 注意点

左右のcapacity追加は既存値への加算。`max_flow` は毎回新しいnetworkを構築し、flowとDinicの正逆辺列を返す。Result頂点idは左 `[0,L)`, 右 `[L,L+R)`, source `L+R`, sink `L+R+1`。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error`。容量・cost・総和の演算結果が `T` に収まることを前提とする。記載した計算量には引数検査とResultの構築を含む。
