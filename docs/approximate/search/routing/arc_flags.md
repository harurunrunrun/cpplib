---
title: Arc Flags (アークフラグ)
documentation_of: ../../../../src/approximate/search/routing/arc_flags.hpp
---

非負重み付き有向グラフを地域に分割し、「その地域内の頂点への最短路に使われる弧」をビットで前計算する。問い合わせでは目的地の地域フラグがない弧を除外するため、返す距離は近似値ではなく厳密な最短距離である。

グラフ型 `graph_type` は
`std::vector<std::vector<std::pair<int, Cost>>>`。各 `pair` は
`(行き先, 非負コスト)` を表す。`cost_type` は `Cost` の別名である。

## 公開 API

```cpp
ArcFlagsRouting<Cost>(
    graph_type graph,
    std::vector<int> region_of_vertex,
    int region_count = -1
);
```

各頂点の地域番号は 0 以上。`region_count == -1` なら最大地域番号から推定する。各目的頂点から逆向き Dijkstra を行い、最短路条件
`d(u,t) == w(u,v) + d(v,t)` を満たす各弧に目的地域のフラグを立てる。

```cpp
int vertex_count() const noexcept;
int region_count() const noexcept;
int region_of(int vertex) const;
bool arc_allows_region(int from, std::size_t edge_index, int region) const;
std::optional<Cost> query_distance(int source, int target) const;
```

- `vertex_count`, `region_count`, `region_of`: 前計算済み分割の情報を返す。
- `arc_allows_region`: 入力時の `graph[from][edge_index]` に対応する弧のフラグを調べる。
- `query_distance`: フラグで枝刈りした Dijkstra。到達不能なら `std::nullopt`。

## 時間計算量

頂点数を $V$、弧数を $E$、地域数を $R$ とする。

- 構築: 時間 $O(V(E+V)\log V)$、空間 $O(E\lceil R/64\rceil+E+V)$
- 各情報アクセサ: `arc_allows_region` を含め $O(1)$
- 問い合わせ: 実際に許可された弧数を $E_f$、到達頂点数を $V_f$ として
  $O((E_f+V_f)\log V_f)$、追加空間 $O(V)$

## 注意点

- 負辺、範囲外頂点、非有限浮動小数コストは `std::invalid_argument`。
- フラグは入力弧ごとに保持するので、多重辺も別々に扱う。
- 浮動小数コストでは最短路等式を厳密比較する。丸め誤差を避けたい用途では整数化した重みを推奨する。
- コスト和が `numeric_limits<Cost>::max()` を超える経路は到達不能相当に飽和する。
- 地域分割は正しさを変えないが、良い地理的分割ほど問い合わせで多くの弧を除外しやすい。

## English

`ArcFlagsRouting` preprocesses, for every input arc, the destination regions for which that arc belongs to a shortest path. Its query is an exact Dijkstra restricted to arcs carrying the target-region flag. Construction takes $O(V(E+V)\log V)$ time and $O(E\lceil R/64\rceil+E+V)$ space; a query takes $O((E_f+V_f)\log V_f)$.
