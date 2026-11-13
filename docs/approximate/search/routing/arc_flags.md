---
title: Arc Flags (アークフラグ)
documentation_of: ../../../../src/approximate/search/routing/arc_flags.hpp
---

非負重み付き有向グラフを地域に分割し、「その地域内の頂点への最短路に使われる弧」をビットで前計算する。問い合わせでは目的地の地域フラグがない弧を除外するため、返す距離は近似値ではなく厳密な最短距離である。

グラフ型 `graph_type` は `std::vector<std::vector<std::pair<int, Cost>>>` で、各 `pair` は `(行き先, 非負コスト)` を表す。`cost_type` は `Cost` の別名である。

## 公開 API

```cpp
ArcFlagsRouting<Cost>(
    graph_type graph,
    std::vector<int> region_of_vertex,
    int region_count = -1
);
```

各頂点の地域番号は 0 以上でなければならない。`region_count` が負（デフォルトは `-1`）なら、最大の地域番号から地域数を推定する。各目的頂点から逆向きの Dijkstra 法を行い、最短路の条件 $d(u,t) = w(u,v) + d(v,t)$ を満たす各弧に、目的頂点の地域のフラグを立てる。

```cpp
int vertex_count() const noexcept;
int region_count() const noexcept;
int region_of(int vertex) const;
bool arc_allows_region(int from, std::size_t edge_index, int region) const;
std::optional<Cost> query_distance(int source, int target) const;
```

- `vertex_count`, `region_count`, `region_of`: 前計算した分割の情報を返す。
- `arc_allows_region`: 入力の `graph[from][edge_index]` に対応する弧のフラグを調べる。
- `query_distance`: フラグで枝刈りした Dijkstra 法で距離を求める。到達不能なら `std::nullopt` を返す。

## 時間計算量

頂点数を $V$、弧数を $E$、地域数を $R$ とする。

- 構築: 時間計算量 $O(V(E+V)\log V)$、空間計算量 $O(E\lceil R/64\rceil+E+V)$
- 各アクセサ: `arc_allows_region` を含めて $O(1)$
- 問い合わせ: 実際に許可された弧数を $E_f$、到達頂点数を $V_f$ として、時間計算量 $O((E_f+V_f)\log V_f)$、追加空間計算量 $O(V)$

## 注意点

- 負の辺、範囲外の頂点、有限でない浮動小数点のコスト、負の地域番号、全地域を覆わない `region_count` に対しては `std::invalid_argument` を送出する。`arc_allows_region` の地域番号が範囲外なら `std::out_of_range` を送出する。
- フラグは入力弧ごとに保持するので、多重辺も別々に扱う。
- 浮動小数点のコストでは、最短路の等式を厳密に比較する。丸め誤差を避けたい用途では、整数化した重みを推奨する。
- コストの和が `numeric_limits<Cost>::max()` を超える経路は、到達不能と同じ扱いに飽和する。
- 地域分割は正しさに影響しないが、地理的に良い分割ほど問い合わせで多くの弧を除外しやすい。
