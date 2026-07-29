---
title: MM Search (中間一致探索)
documentation_of: ../../../src/approximate/search/mm_search.hpp
---

MM Search は、双方向探索の各状態に

$$
\operatorname{pr}(v)=\max(g(v)+h(v),\,2g(v))
$$

を与え、最小 priority の方向を展開する。`2g` により、必要になる前に探索が最短解の中点を越えることを防ぐ。

## API

```cpp
mm_search(
    start, goal,
    forward_neighbors, backward_neighbors,
    forward_heuristic, backward_heuristic,
    hash = {}, equal = {}
)
```

近傍 callable は `pair<State, Cost>` の range を返す。`forward_heuristic(v)` は `v` から `goal`、`backward_heuristic(v)` は `start` から `v` の非負な下界。後者は逆向き探索における残り距離である。返り値は `HeuristicSearchResult<State, Cost>`。

終了判定には両方向の最小 priority、最小 $f$、最小 $g$ の和を使う。両ヒューリスティックが許容的かつ整合的なら最短経路を返す。

両方向合計の生成辺数を $A$、展開数を $X$、発見状態数を $V_d$ とすると、3本ずつの遅延削除ヒープを用いるため時間計算量は $O((A+X)\log V_d)$、空間計算量は $O(A+V_d)$。改善エントリを含むヒープ要素総数は $O(A)$。

## 注意点

- 全辺コストと両ヒューリスティックは非負でなければならない。
- `backward_neighbors` は元の有向辺を逆向きに、同じコストで列挙する。
- 最短性の保証には固定端点への許容的・整合的ヒューリスティックが必要。ゼロヒューリスティックは安全。
- コスト、`g+h`、`2g` は `common_type_t` の表現可能範囲に収まる必要がある。

## English

MM Search orders a state by `max(g+h, 2g)` and expands the direction with the globally smallest priority. The `2g` term gives the meet-in-the-middle property. Forward and backward heuristics estimate the distance to their opposite fixed endpoints.

With non-negative consistent admissible heuristics, `mm_search` returns an optimal `HeuristicSearchResult<State, Cost>`. For $A$ generated arcs, $X$ expansions, and $V_d$ discovered states, time is $O((A+X)\log V_d)$ and space is $O(A+V_d)$. Negative arcs or heuristic values throw `std::invalid_argument`.
