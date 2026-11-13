---
title: Hub Labeling (ハブラベリング)
documentation_of: ../../../../src/approximate/search/routing/hub_labeling.hpp
---

各頂点 $v$ に前向きラベル `(hub, d(v, hub))` と後ろ向きラベル `(hub, d(hub, v))` を持たせる、有向グラフの 2-hop cover である。重要度の順に枝刈り付き Dijkstra 法（pruned Dijkstra）を行い、既存のラベルで同じ距離以下を被覆できる頂点では探索を打ち切る。問い合わせでは、2 つのラベルに共通するハブを線形時間のマージで求める。

`graph_type` は `std::vector<std::vector<std::pair<int, Cost>>>`、`cost_type` は `Cost` の別名である。

## 公開 API

```cpp
struct LabelEntry {
    int hub;
    Cost distance;
};

explicit HubLabeling(
    graph_type graph,
    std::vector<int> hub_order = {}
);
```

`hub_order` は、先に処理するハブから順に並べた全頂点の順列である。空なら、入次数と出次数の和の大きい順に安定ソートした順を使う。

```cpp
int vertex_count() const noexcept;
const std::vector<LabelEntry>& forward_labels(int vertex) const;
const std::vector<LabelEntry>& backward_labels(int vertex) const;
std::optional<Cost> query_distance(int source, int target) const;
```

- `forward_labels(v)`: `v -> hub` のラベル。
- `backward_labels(v)`: `hub -> v` のラベル。
- `query_distance`: 両方のラベルに現れるハブについて、距離の和の最小値を返す。到達不能なら `std::nullopt` を返す。

ラベルは `hub_order` の順に格納され、頂点番号順とは限らない。

## 時間計算量

頂点数を $V$、辺数を $E$、全ラベル数を $L$、問い合わせる 2 つのラベルの長さを $L_s$, $L_t$ とする。

- 構築: 最悪時間計算量 $O(V(E+V)\log V+V^3)$、空間計算量 $O(E+L)$
- `vertex_count`, `forward_labels`, `backward_labels`: $O(1)$
- `query_distance`: 時間計算量 $O(L_s+L_t)$、追加空間計算量 $O(1)$

$V^3$ の項は、前処理中のラベルの共通部分の検査に対する最悪の上界であり、枝刈りがよく効く道路網では通常かなり小さくなる。

## 注意点

- 順序はラベル数と構築時間を大きく左右するが、正しさには影響しない。
- 有向グラフでは、前向きラベルと後ろ向きラベルは交換できない。
- 返すのは距離だけで、経路を復元する API は提供しない。
- 負の辺、有限でないコスト、不正な順序に対しては `std::invalid_argument` を送出する。
- コスト 0 の辺と多重辺を許す。
