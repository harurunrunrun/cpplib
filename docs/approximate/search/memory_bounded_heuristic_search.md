---
title: Memory-Bounded Heuristic Search (メモリ制限ヒューリスティック探索)
documentation_of: ../../../src/approximate/search/memory_bounded_heuristic_search.hpp
---

OPEN に保持する候補数へ明示的な上限を設ける A* 系探索を提供する。
`neighbors(state)` は `pair<State, Cost>` の range、`goal(state)` は真偽値、
`heuristic(state)` は非負の推定コストを返す。

## `simplified_memory_bounded_a_star_search`

```cpp
simplified_memory_bounded_a_star_search(
    start, goal, neighbors, heuristic, max_frontier_size,
    hash = {}, equal = {}
)
```

SMA* の優先規則を用いる。子の $f$ 値へ親のバックアップ値を伝播し、OPEN が
`max_frontier_size` を超えると最大 $f$ の浅い候補を削除する。同じ $f$ なら
深い候補を先に展開するため、限られた OPEN を深さ方向へ利用できる。

展開回数を $X$、生成辺数を $A$、上限を $M$、最大経路長を $D$ とすると、
線形な最良・最悪候補選択を含む時間計算量は $O((X+A)M+AD)$、
空間計算量は $O(MD+V_d)$。$V_d$ は重複抑制用に記録した状態数。

## `memory_bounded_a_star_search`

```cpp
memory_bounded_a_star_search(
    start, goal, neighbors, heuristic, max_frontier_size,
    hash = {}, equal = {}
)
```

$g+h$ が最小の候補を展開し、上限超過時は $g+h$ が最大の候補を直ちに捨てる
MA*。SMA* と異なり親のバックアップ値を子へ伝播せず、同値候補は生成順で処理する。

展開回数を $X$、生成辺数を $A$、上限を $M$、最大経路長を $D$ とすると、
時間計算量は $O((X+A)M+AD)$、空間計算量は $O(MD+V_d)$。

## 注意点

- `max_frontier_size == 0`、負辺、負または非有限な推定値には
  `std::invalid_argument` を送出する。
- OPEN 上限が探索に必要な候補数より小さい場合は、解が存在しても未発見を返したり、
  非最適解を返したりし得る。十分な上限と許容的ヒューリスティックの下では
  A* と同じ最適解を返す。
- OPEN の各候補は復元用経路を保持する。引数の上限は OPEN の候補数に対する上限で、
  重複抑制表の状態数は含まない。
- `Cost{}` がゼロを表し、コスト加算と `long double` への変換が表現可能であることを
  前提とする。
