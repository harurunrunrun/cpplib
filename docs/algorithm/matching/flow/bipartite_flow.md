---
title: Bipartite Flow (二部グラフフロー)
documentation_of: ../../../../src/algorithm/matching/flow/bipartite_flow.hpp
---

二部グラフ上の最大流を求める。`source -> left -> right -> sink` の形のネットワークを作り、Dinic 法で最大流を流す。

## コンストラクタ

```cpp
BipartiteFlow<T> graph(left_size, right_size)
```

左側 `left_size` 頂点、右側 `right_size` 頂点の空のネットワークを作る。左右の各頂点の容量は $0$ で初期化される。

## メンバ関数

```cpp
graph.add_left_capacity(left, cap)
graph.add_right_capacity(right, cap)
graph.add_edge(left, right, cap)
graph.max_flow()
```

- `add_left_capacity(left, cap)`: `source -> left` の容量に `cap` を加算する。
- `add_right_capacity(right, cap)`: `right -> sink` の容量に `cap` を加算する。
- `add_edge(left, right, cap)`: 容量 `cap` の辺 `left -> right` を追加する。
- `max_flow()`: 最大流を求め、`BipartiteFlowResult<T>` を返す。

`BipartiteFlowResult<T>` は次のメンバを持つ。

```cpp
T flow;
vector<DinicEdge<T>> edges;
```

`edges` は内部で構築した Dinic のネットワークの辺列（正辺の直後に逆辺）である。辺の追加順は、`source -> left` の辺（左頂点順）、`add_edge` で追加した辺（追加順）、`right -> sink` の辺（右頂点順）である。頂点番号は、左が $[0, L)$、右が $[L, L+R)$、source が $L+R$、sink が $L+R+1$ である。

## 時間計算量

$L, R$ を左右の頂点数、$E$ を左右間に追加した辺数とし、構築するネットワークの頂点数を $V' = L+R+2$、辺数を $E' = L+R+E$ とする。

- `BipartiteFlow(L, R)`: $O(L+R)$
- `add_left_capacity`, `add_right_capacity`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `max_flow`: ネットワーク構築と辺列の出力に $O(V'+E')$、Dinic 法に $O(V'^2 E')$

## 空間計算量

- 保存中の入力、一時的な Dinic のグラフ、戻り値を含めて $O(V'+E')$

## 注意点

`max_flow` は呼び出すたびに新しいネットワークを構築する。

頂点数と容量は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error` を送出する。容量や流量の総和が `T` に収まることを前提とする。記載した計算量には引数検査と戻り値の構築を含む。
