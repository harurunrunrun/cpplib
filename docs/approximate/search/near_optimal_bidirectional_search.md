---
title: NBS (近最適双方向探索)
documentation_of: ../../../src/approximate/search/near_optimal_bidirectional_search.hpp
---

NBS は、双方向探索で「現在の上界より短い解を証明するために展開が必要になり得る」前向き・後ろ向き状態の対を選ぶ。待機集合を $f=g+h$、準備集合を $g$ で管理し、現在の下界 $C$ に対して

$$
f_F(u)\le C,\quad f_B(v)\le C,\quad g_F(u)+g_B(v)\le C
$$

を満たす最小 $g$ の対を同時に展開する。

## API

```cpp
nbs_search(
    start, goal,
    forward_neighbors, backward_neighbors,
    forward_heuristic, backward_heuristic,
    hash = {}, equal = {}
)
```

近傍 callable は `pair<State, Cost>` の range。前向きヒューリスティックは `goal` まで、後ろ向きヒューリスティックは `start` までの下界を返す。返り値は `HeuristicSearchResult<State, Cost>`。

非負で許容的かつ整合的なヒューリスティックでは最短経路を返す。NBS の対選択は、同じ front-to-end 下界を用いる任意の正しい双方向探索が必ず被覆する must-expand pair グラフに対し、最適頂点被覆の高々2倍の状態を展開する保証を持つ。

両方向合計の生成辺数を $A$、展開数を $X$、発見状態数を $V_d$ とすると、時間計算量は $O((A+X)\log V_d)$、空間計算量は $O(A+V_d)$。各状態改善は待機または準備ヒープへ1要素を追加し、古い要素は遅延削除する。

## 注意点

- 辺コストとヒューリスティックは非負でなければならない。
- 有向グラフでは `backward_neighbors` が元の入辺を逆向きに列挙する必要がある。
- 2倍の展開数保証は整合的ヒューリスティックと標準の must-expand lower-bound モデルに対するもの。
- コスト加算と下界計算は型の表現可能範囲に収まる必要がある。

## English

Near-optimal Bidirectional Search maintains waiting queues ordered by `f=g+h` and ready queues ordered by `g`. At lower bound `C`, it expands a forward/backward pair satisfying `f_F<=C`, `f_B<=C`, and `g_F+g_B<=C`.

`nbs_search` returns an optimal `HeuristicSearchResult<State, Cost>` under non-negative consistent admissible heuristics. Its pair selection expands at most twice the minimum vertex cover of the standard must-expand pair graph. Time is $O((A+X)\log V_d)$ and space is $O(A+V_d)$. Negative arcs or estimates throw `std::invalid_argument`.
