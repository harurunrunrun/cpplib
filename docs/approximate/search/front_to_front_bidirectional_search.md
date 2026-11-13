---
title: Front-to-Front Bidirectional Search (フロント間双方向探索)
documentation_of: ../../../src/approximate/search/front_to_front_bidirectional_search.hpp
---

始点側と終点側のフロンティアの**状態の対**に下界を付ける双方向探索である。固定端点へのヒューリスティックだけを使う Front-to-End 方式とは異なり、現在の両フロンティア間の推定距離を直接使う。

## API

```cpp
front_to_front_bidirectional_search(
    start, goal,
    forward_neighbors, backward_neighbors,
    front_to_front_heuristic,
    hash = {}, equal = {}
)
```

`forward_neighbors(v)` は元のグラフの出辺を、`backward_neighbors(v)` は元のグラフの入辺を逆向きにたどる `pair<State, Cost>` の範囲を返す。`front_to_front_heuristic(u, v)` は `u` から `v` までの距離の非負の下界を返す。返値は `HeuristicSearchResult<State, Cost>`。

未展開の対を

$$
g_F(u)+h(u,v)+g_B(v)
$$

の昇順で管理し、下界が最小の対の片側を交互に展開する。両側で発見済みの同一状態から暫定解を更新する。ヒューリスティックがすべてのフロンティア対について許容的なら最短経路を返す。整合的でない下界のために状態が改善された場合は、その状態を再オープンする。

生成辺数を $A$、挿入されたフロンティア対の総数（無効になったものを含む）を $R$、同時保持する対の最大数を $P$ とすると、時間計算量は $O(A+R\log P)$、空間計算量は発見状態数 $V_d$ に対して $O(V_d+P)$。対ヒューリスティックの呼び出し回数は $R$ である。フロンティアが密な場合は $P=O(V_F V_B)$ になり得る。

## 注意点

- 辺のコストと対ヒューリスティックは非負でなければならず、違反時は `std::invalid_argument` を送出する。
- `backward_neighbors` の辺コストは対応する元の辺と同じでなければならない。
- 最短性には $h(u,v) \le d(u,v)$ が必要である。$h = 0$ は常に安全である。
- コストの加算は型の表現範囲に収まらなければならない。
- 有限グラフを前提とし、ハッシュ表の操作は期待 $O(1)$ と仮定する。
