---
title: Reach-Based Routing (リーチベース経路探索)
documentation_of: ../../../../src/approximate/search/routing/reach_based_routing.hpp
---

頂点 $v$ の reach は、その頂点を通る最短路上で端点からどれだけ内側に位置し得るかを表す。問い合わせ中の確定距離 $g(v)$ と目的地までの許容下界 $h(v,t)$ がともに reach 上界を超えれば、$v$ は最短路上にないので安全に枝刈りできる。

`graph_type` は `std::vector<std::vector<std::pair<int, Cost>>>`、
`cost_type` は `Cost` の別名である。

## 公開 API

```cpp
explicit ReachBasedRouting(graph_type graph);
```

全始点 Dijkstra を行い、各頂点について
`min(max_s d(s,v), max_t d(v,t))` という安全な reach 上界と、問い合わせ用の厳密な `d(v,t)` 下界表を作る。厳密 reach の三重ループ計算は行わない。

```cpp
ReachBasedRouting(
    graph_type graph,
    std::vector<Cost> reach_upper_bounds
);
```

外部で作った reach 上界を受け取る高速構築経路。値は真の reach 以上でなければならない。この構築では全点対距離表を作らない。

```cpp
int vertex_count() const noexcept;
Cost reach_upper_bound(int vertex) const;
bool has_precomputed_target_bounds() const noexcept;
```

頂点数、頂点の reach 上界、組み込みの目的地下界表を持つかを返す。

```cpp
std::optional<Cost> query_distance(int source, int target) const;
```

組み込み構築なら厳密な `d(v,t)` を使う。外部 reach 構築なら下界 0 を使うため枝刈りはしないが、正確な Dijkstra として動作する。

```cpp
template<class RemainingDistanceLowerBound>
std::optional<Cost> query_distance(
    int source,
    int target,
    RemainingDistanceLowerBound lower_bound
) const;
```

`lower_bound(vertex, target)` を呼ぶ reach 枝刈り問い合わせ。下界は 0 以上かつ真の `d(vertex,target)` 以下でなければならない。到達不能なら `std::nullopt`。

## 時間計算量

$V,E$ は頂点・弧数、下界1回の時間を $H$ とする。

- 組み込み構築: 時間 $O(V(E+V)\log V)$、空間 $O(V^2+E)$
- 外部 reach 構築: 時間・空間とも入力保存を含め $O(V+E)$
- `vertex_count`, `has_precomputed_target_bounds`: $O(1)$
- `reach_upper_bound`: $O(1)$
- 問い合わせ: 展開頂点数 $V_r$、調査弧数 $E_r$ に対し
  $O((E_r+V_r)\log V_r+V_rH)$、追加空間 $O(V)$

## 注意点

- 外部値は reach の下界ではなく、安全な上界を渡す。小さ過ぎる値は誤った枝刈りを起こす。
- callable の値は許容下界でなければならない。過大評価は正しさを失わせる。
- 負の reach／下界は `std::invalid_argument`。
- 組み込み上界は安全だが、道路網専用の reach 前処理より緩い場合がある。
- 辺コストは非負かつ有限でなければならない。

## English

`ReachBasedRouting` prunes a settled vertex only when both its source distance and an admissible remaining-distance lower bound exceed a safe reach upper bound. The graph-only constructor builds safe bounds and exact target bounds in $O(V(E+V)\log V)$ time. The second constructor accepts externally preprocessed reach upper bounds in $O(V+E)$; supply an admissible callable to the three-argument query to enable pruning.
