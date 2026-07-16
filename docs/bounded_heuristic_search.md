---
title: Bounded Heuristic Search (制限付きヒューリスティック探索)
documentation_of: ../src/approximate/search/bounded_heuristic_search.hpp
---

探索幅またはメモリを抑えるヒューリスティック探索。

## `beam_search`

```cpp
beam_search(start, goal, neighbors, heuristic,
            beam_width, maximum_depth, hash = {}, equal = {})
```

各深さで $g+h$ が小さい状態を高々 `beam_width` 個だけ残す。幅を制限するため、経路が存在しても見つからないことや、非最短経路を返すことがある。全辺コストは非負、`beam_width > 0` が必要。返り値は `HeuristicSearchResult<State, Cost>`。

分岐数の最大を $b$、幅を $B$、深さ上限を $D$ とすると、候補のソートを含む時間計算量は $O(DBb\log(Bb))$。復元用に発見した状態を残すため、空間計算量は $O(DBb)$。

## `ida_star_search`

```cpp
ida_star_search(start, goal, neighbors, heuristic,
                hash = {}, equal = {})
```

$f=g+h$ の上限を反復的に引き上げる深さ優先探索。現在の経路上の重複状態を除去する。非負辺と非負ヒューリスティックが必要で、許容的なヒューリスティックなら最初に見つける経路は最短になる。

最適解の深さを $d$、分岐数を $b$ とした典型的な時間計算量は $O(b^d)$。反復回数とヒューリスティックに依存して同じ状態を何度も展開する。探索作業領域は $O(d)$、返す経路も $O(d)$。各経路内重複判定はハッシュ表の期待 $O(1)$ と仮定する。

## 注意点

`neighbors(state)` は `pair<State, Cost>` の range、`goal(state)` は真偽値、`heuristic(state)` は推定残りコストを返す。負辺、Beam Search の幅0、IDA* の負ヒューリスティックは `std::invalid_argument`。コスト加算は型の表現範囲に収まる必要がある。
