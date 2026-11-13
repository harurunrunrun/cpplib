---
title: NBS (近最適双方向探索)
documentation_of: ../../../src/approximate/search/near_optimal_bidirectional_search.hpp
---

NBS は、双方向探索で「現在の上界より短い解を証明するために展開が必要になり得る」前向き・後ろ向きの状態の対を選ぶ。待機キューを $f=g+h$ で、準備キューを $g$ で順序付けて管理し、現在の下界 $C$ に対して

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

近傍関数は `pair<State, Cost>` の範囲を返す。前向きヒューリスティックは `goal` まで、後ろ向きヒューリスティックは `start` までの距離の下界を返す。返値は `HeuristicSearchResult<State, Cost>`。

非負で許容的かつ整合的なヒューリスティックなら最短経路を返す。NBS の対の選び方は、同じ front-to-end 下界を使う任意の正しい双方向探索が必ず被覆しなければならない must-expand pair グラフについて、最小頂点被覆の高々 2 倍の状態しか展開しないことが保証されている。

両方向合計の生成辺数を $A$、展開数を $X$、発見状態数を $V_d$ とすると、時間計算量は $O((A+X)\log V_d)$、空間計算量は $O(A+V_d)$。状態が改善されるたびに待機ヒープまたは準備ヒープへ 1 要素を追加し、古い要素は遅延削除する。

## 注意点

- 辺のコストとヒューリスティックは非負でなければならず、違反時は `std::invalid_argument` を送出する。
- 有向グラフでは `backward_neighbors` が元の入辺を逆向きに列挙する必要がある。
- 展開数が 2 倍以内という保証は、整合的なヒューリスティックと標準的な must-expand 下界モデルについてのものである。
- コストの加算と下界の計算は型の表現範囲に収まらなければならない。
