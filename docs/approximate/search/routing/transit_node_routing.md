---
title: Transit Node Routing (トランジットノード経路探索)
documentation_of: ../../../../src/approximate/search/routing/transit_node_routing.hpp
---

長距離経路が通りやすい少数の transit node を指定し、各頂点と transit の間の access 距離および transit 間距離を前計算する。問い合わせでは transit 表の組合せから候補距離を求め、transit を通らない最短路の可能性が残る局所的な問い合わせに限り局所 Dijkstra の結果と比較する。したがって返す距離は厳密である。

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

全 transit から順方向・逆方向の Dijkstra を行う。この実装の access 集合は到達可能な全 transit を保持するので、距離の最小化で候補を取りこぼさない。

```cpp
int vertex_count() const noexcept;
std::size_t transit_node_count() const noexcept;
const std::vector<int>& transit_nodes() const noexcept;
const std::vector<AccessNode>& forward_access_nodes(int vertex) const;
const std::vector<AccessNode>& backward_access_nodes(int vertex) const;
```

`forward_access_nodes(v)` は `v -> transit`、`backward_access_nodes(v)` は
`transit -> v` の有限距離の一覧を返す。残りは頂点数と transit 集合のアクセサである。

```cpp
bool is_definitely_nonlocal(int source, int target) const;
std::optional<Cost> query_distance(int source, int target) const;
```

transit を除いたグラフで両端点の弱連結成分が異なる、または端点自身が transit なら `is_definitely_nonlocal` は真を返す。`query_distance` はこの十分条件が真なら表引きだけを使い、偽なら transit を経由しない局所 Dijkstra の値との最小を返す。

## 時間計算量

transit 数を $K$、各端点の access 数を $A_s,A_t$ とする。

- 構築: 時間 $O(K(E+V)\log V+KV+K^2)$、空間 $O(KV+E+V)$
- access／集合アクセサ、`vertex_count`, `transit_node_count`: $O(1)$
- `is_definitely_nonlocal`: $O(1)$
- 非局所問い合わせ: $O(A_sA_t)$
- 局所問い合わせ: 上記に加え、局所探索した弧・頂点を $E_l,V_l$ として
  $O((E_l+V_l)\log V_l)$。問い合わせ追加空間は $O(V)$

## 注意点

- transit node は重複のない有効な頂点番号でなければならない。違反すると `std::invalid_argument` を投げる。
- 範囲外の頂点を問い合わせると `std::out_of_range` を投げる。
- transit が空でも正しく動作し、全問い合わせを局所 Dijkstra で処理する。
- 弱連結成分フィルタは保守的で、偽でも実際には非局所の場合がある。正しさには影響しない。
- transit 選択は正しさを変えないが、少な過ぎると局所探索が増え、多過ぎると $KV$ の前計算と $A_sA_t$ が増える。
- 辺コストは非負かつ有限でなければならない。

## English

`TransitNodeRouting` precomputes distances to, from, and between caller-selected transit nodes. A conservative weak-component locality filter permits an $O(A_sA_t)$ table query when a transit-free route is impossible; otherwise a transit-free local Dijkstra is compared with the table candidate. The result is always exact.
