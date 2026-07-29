---
title: Transit Node Routing (トランジットノード経路探索)
documentation_of: ../../../../src/approximate/search/routing/transit_node_routing.hpp
---

長距離経路が通りやすい少数の transit node を指定し、各頂点から／への access 距離と transit 間距離を前計算する。問い合わせは transit 表の組合せを計算し、transit を通らない可能性が残る局所問い合わせだけ局所 Dijkstra と比較するため厳密である。

`graph_type` は `std::vector<std::vector<std::pair<int, Cost>>>`、
`cost_type` は `Cost` の別名である。

## 公開 API

```cpp
struct AccessNode {
    int transit_vertex;
    Cost distance;
};

TransitNodeRouting(
    graph_type graph,
    std::vector<int> transit_nodes
);
```

全 transit から通常向き・逆向き Dijkstra を行う。この実装の access 集合は到達可能な全 transit を保持し、距離最小化時の取りこぼしを避ける。

```cpp
int vertex_count() const noexcept;
std::size_t transit_node_count() const noexcept;
const std::vector<int>& transit_nodes() const noexcept;
const std::vector<AccessNode>& forward_access_nodes(int vertex) const;
const std::vector<AccessNode>& backward_access_nodes(int vertex) const;
```

`forward_access_nodes(v)` は `v -> transit`、`backward_access_nodes(v)` は
`transit -> v` の有限距離を返す。他は頂点数と transit 集合のアクセサ。

```cpp
bool is_definitely_nonlocal(int source, int target) const;
std::optional<Cost> query_distance(int source, int target) const;
```

transit を除いたグラフの弱連結成分が異なる、または端点自身が transit なら
`is_definitely_nonlocal` は真。この十分条件で真なら表引きだけを使い、偽なら
transit を内部に含まない局所 Dijkstra の値との最小を返す。

## 時間計算量

transit 数を $K$、各端点の access 数を $A_s,A_t$ とする。

- 構築: 時間 $O(K(E+V)\log V+KV+K^2)$、空間 $O(KV+E+V)$
- access／集合アクセサ、`vertex_count`, `transit_node_count`: $O(1)$
- `is_definitely_nonlocal`: $O(1)$
- 非局所問い合わせ: $O(A_sA_t)$
- 局所問い合わせ: 上記に加え、局所探索した弧・頂点を $E_l,V_l$ として
  $O((E_l+V_l)\log V_l)$。問い合わせ追加空間は $O(V)$

## 注意点

- transit node は重複のない有効な頂点番号でなければならない。
- transit が空でも正しく動作し、全問い合わせを局所 Dijkstra で処理する。
- 弱連結成分フィルタは保守的で、偽でも実際には非局所の場合がある。正しさには影響しない。
- transit 選択は正しさを変えないが、少な過ぎると局所探索が増え、多過ぎると $KV$ の前計算と $A_sA_t$ が増える。
- 辺コストは非負かつ有限でなければならない。

## English

`TransitNodeRouting` precomputes distances to, from, and between caller-selected transit nodes. A conservative weak-component locality filter permits an $O(A_sA_t)$ table query when a transit-free route is impossible; otherwise a transit-free local Dijkstra is compared with the table candidate. The result is always exact.
