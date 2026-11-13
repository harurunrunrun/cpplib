---
title: Bounded Heuristic Search (制限付きヒューリスティック探索)
documentation_of: ../../../src/approximate/search/bounded_heuristic_search.hpp
---

探索幅またはメモリ使用量を抑えるヒューリスティック探索である。

## `beam_search`

```cpp
beam_search(start, goal, neighbors, heuristic,
            beam_width, maximum_depth, hash = {}, equal = {})
```

各深さで $g+h$ が小さい状態を高々 `beam_width` 個だけ残す。幅を制限するため、経路が存在しても見つからないことや、最短でない経路を返すことがある。全辺のコストが非負で、`beam_width > 0` でなければならない。返値は `HeuristicSearchResult<State, Cost>`。

分岐数の最大を $b$、幅を $B$、深さ上限を $D$ とすると、候補のソートを含む時間計算量は $O(DBb\log(Bb))$。復元用に発見した状態を残すため、空間計算量は $O(DBb)$。

## `ida_star_search`

```cpp
ida_star_search(start, goal, neighbors, heuristic,
                hash = {}, equal = {})
```

$f=g+h$ の上限を反復的に引き上げる深さ優先探索である。現在の経路上で重複する状態は除く。辺のコストとヒューリスティックは非負でなければならず、許容的なヒューリスティックなら最初に見つかる経路は最短になる。

最適解の深さを $d$、分岐数を $b$ とした典型的な時間計算量は $O(b^d)$。反復回数とヒューリスティックによっては、同じ状態を何度も展開する。探索の作業領域は $O(d)$、返す経路も $O(d)$。経路内の重複判定は、ハッシュ表の期待 $O(1)$ 操作と仮定する。

## 注意点

- `neighbors(state)` は `pair<State, Cost>` の範囲（range）、`goal(state)` は真偽値、`heuristic(state)` は推定残りコストを返す。
- 負の辺、Beam Search の幅 0、IDA* の負のヒューリスティック値に対しては `std::invalid_argument` を送出する。
- コストの加算は型の表現範囲に収まらなければならない。
