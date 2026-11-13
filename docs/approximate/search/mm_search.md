---
title: MM Search (中間一致探索)
documentation_of: ../../../src/approximate/search/mm_search.hpp
---

MM Search は、双方向探索の各状態に

$$
\operatorname{pr}(v)=\max(g(v)+h(v),\,2g(v))
$$

を与え、優先度が最小の方向を展開する。$2g$ の項により、探索が必要になる前に最短解の中点を越えることを防ぐ（meet-in-the-middle 性）。

## API

```cpp
mm_search(
    start, goal,
    forward_neighbors, backward_neighbors,
    forward_heuristic, backward_heuristic,
    hash = {}, equal = {}
)
```

近傍関数は `pair<State, Cost>` の範囲を返す。`forward_heuristic(v)` は `v` から `goal` まで、`backward_heuristic(v)` は `start` から `v` までの距離の非負の下界を返す。後者は逆向き探索における残り距離である。返値は `HeuristicSearchResult<State, Cost>`。

終了判定には、両方向の最小優先度、最小 $f$、最小 $g$ の和を使う。両ヒューリスティックが許容的かつ整合的なら最短経路を返す。

両方向合計の生成辺数を $A$、展開数を $X$、発見状態数を $V_d$ とすると、各方向 3 本の遅延削除ヒープを使うため時間計算量は $O((A+X)\log V_d)$、空間計算量は $O(A+V_d)$。改善エントリを含むヒープ要素総数は $O(A)$。

## 注意点

- 全辺コストと両ヒューリスティックは非負でなければならない。
- `backward_neighbors` は元の有向辺を逆向きに、同じコストで列挙する。
- 負の辺やヒューリスティック値に対しては `std::invalid_argument` を送出する。
- 最短性の保証には、固定端点への許容的かつ整合的なヒューリスティックが必要である。常に 0 を返すヒューリスティックは安全である。
- コスト、$g+h$、$2g$ は `common_type_t` の表現範囲に収まらなければならない。
