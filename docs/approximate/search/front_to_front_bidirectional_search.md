---
title: Front-to-Front Bidirectional Search (フロント間双方向探索)
documentation_of: ../../../src/approximate/search/front_to_front_bidirectional_search.hpp
---

始点側と終点側の frontier の**状態対**に下界を付ける双方向探索。固定端点へのヒューリスティックだけを使う Front-to-End 方式とは異なり、現在の両 frontier 間の推定距離を直接使う。

## API

```cpp
front_to_front_bidirectional_search(
    start, goal,
    forward_neighbors, backward_neighbors,
    front_to_front_heuristic,
    hash = {}, equal = {}
)
```

`forward_neighbors(v)` は元グラフの出辺、`backward_neighbors(v)` は元グラフの入辺を逆向きに辿る `pair<State, Cost>` の range を返す。`front_to_front_heuristic(u, v)` は `u` から `v` までの非負な下界を返す。返り値は `HeuristicSearchResult<State, Cost>`。

未展開対を

$$
g_F(u)+h(u,v)+g_B(v)
$$

の昇順で管理し、最小下界の対の片側を交互に展開する。両側で発見済みの同一状態から暫定解を更新する。ヒューリスティックが全ての frontier 対について許容的なら最短経路を返す。不整合な下界による改善時は状態を再オープンする。

生成辺数を $A$、挿入された有効・無効を含む frontier 対の総数を $R$、同時保持する対の最大数を $P$ とすると、時間計算量は $O(A+R\log P)$、空間計算量は発見状態数 $V_d$ に対して $O(V_d+P)$。対ヒューリスティックの呼び出し回数は $R$。密な frontier では $P=O(V_FV_B)$ になり得る。

## 注意点

- 辺コストと対ヒューリスティックは非負でなければならない。
- `backward_neighbors` の辺コストは対応する元の辺と同じでなければならない。
- 最短性には `h(u,v) <= d(u,v)` が必要。`h = 0` は常に安全。
- コストの加算は型の表現可能範囲に収まる必要がある。
- 有限グラフを前提とし、ハッシュ表操作は期待 $O(1)$ と仮定する。

## English

This bidirectional search assigns a lower bound to a **pair** of current forward and backward frontier states. `forward_neighbors` traverses original arcs, `backward_neighbors` traverses reversed original arcs, and `front_to_front_heuristic(u, v)` estimates the remaining distance from `u` to `v`.

The algorithm orders frontier pairs by `g_F(u) + h(u,v) + g_B(v)` and alternates which endpoint of the best pair it expands. It returns `HeuristicSearchResult<State, Cost>`. With a non-negative admissible pair heuristic it returns an optimal path; improved states are reopened.

For $A$ generated arcs, $R$ inserted pair records, and maximum pair heap size $P$, time is $O(A+R\log P)$ and space is $O(V_d+P)$. A dense pair frontier can require $P=O(V_FV_B)$. Negative arcs or estimates throw `std::invalid_argument`.
