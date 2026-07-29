---
title: Hub Labeling (ハブラベリング)
documentation_of: ../../../../src/approximate/search/routing/hub_labeling.hpp
---

各頂点 $v$ に forward label `(hub, d(v,hub))` と backward label
`(hub, d(hub,v))` を持たせる directed 2-hop cover。重要度順に pruned Dijkstra を行い、既存ラベルで同距離以下を被覆できる頂点では探索を打ち切る。問い合わせは2ラベルの共通 hub を線形 merge する。

`graph_type` は `std::vector<std::vector<std::pair<int, Cost>>>`、
`cost_type` は `Cost` の別名である。

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

`hub_order` は先に処理する hub から並べた全頂点の置換。空なら入次数＋出次数の大きい順を安定ソートする。

```cpp
int vertex_count() const noexcept;
const std::vector<LabelEntry>& forward_labels(int vertex) const;
const std::vector<LabelEntry>& backward_labels(int vertex) const;
std::optional<Cost> query_distance(int source, int target) const;
```

- `forward_labels(v)`: `v -> hub` のラベル。
- `backward_labels(v)`: `hub -> v` のラベル。
- `query_distance`: 両ラベルに現れる hub について距離和の最小を返す。到達不能なら `std::nullopt`。

ラベルは `hub_order` 順に格納される。頂点番号順とは限らない。

## 時間計算量

全ラベル数を $L$、問い合わせ2ラベルの長さを $L_s,L_t$ とする。

- 構築: 最悪時間 $O(V(E+V)\log V+V^3)$、空間 $O(E+L)$
- `vertex_count`, `forward_labels`, `backward_labels`: $O(1)$
- `query_distance`: 時間 $O(L_s+L_t)$、追加空間 $O(1)$

$V^3$ 項は前処理中のラベル共通部分検査に対する疎でない最悪上界で、pruning が有効な道路網では通常かなり小さくなる。

## 注意点

- 順序はラベル数と構築時間を大きく左右するが、正しさは変えない。
- 有向グラフでは forward/backward ラベルを交換できない。
- 返すのは距離で、経路復元 API は提供しない。
- 負辺、非有限コスト、不正な順序は `std::invalid_argument`。
- ゼロコスト辺と多重辺を許す。

## English

`HubLabeling` builds an exact directed 2-hop cover using pruned forward and reverse Dijkstra searches. The query linearly intersects the source forward label and target backward label in $O(L_s+L_t)$ time and $O(1)$ extra space. The supplied hub order affects label size but not correctness.
