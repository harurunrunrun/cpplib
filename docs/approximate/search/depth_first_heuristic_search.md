---
title: Depth-First Heuristic Search (深さ優先ヒューリスティック探索)
documentation_of: ../../../src/approximate/search/depth_first_heuristic_search.hpp
---

探索経路だけを主に保持する、メモリ使用量の小さいヒューリスティック探索を提供する。
共通して `neighbors(state)` は `pair<State, Cost>` の range、`goal(state)` は真偽値、
`heuristic(state)` はゴールまでの非負の推定コストを返す。

## `weighted_ida_star_search`

```cpp
weighted_ida_star_search(
    start, goal, neighbors, heuristic, weight, hash = {}, equal = {}
)
```

$f_w=g+w h$ の上限を、直前の反復で上限を超えた最小値へ更新しながら
深さ優先探索を繰り返す Weighted IDA*。`weight == 1` なら IDA* であり、
許容的ヒューリスティックの下で最適解を返す。`weight > 1` では探索量を減らせる
一方、最適性は一般には保証されない。

1 回の反復で調べる辺数を $A_i$、展開状態数を $X_i$、最大探索深さを $D$ とする。
時間計算量は $O(\sum_i(A_i+X_i))$、空間計算量は $O(D)$。
経路上の重複判定に平均 $O(1)$ のハッシュ表を用いる。

## `recursive_best_first_search`

```cpp
recursive_best_first_search(
    start, goal, neighbors, heuristic, hash = {}, equal = {}
)
```

最良の後続を再帰的に探索し、代替候補の $f$ 値を再帰上限として渡す RBFS。
子の $f$ には親の $f$ との最大値を伝播し、打ち切った部分木の最良値を親へ
バックアップする。許容的ヒューリスティックの下で最適解を返す。

展開回数を $X$、各展開の分岐数を $b_v$、最大深さを $D$ とすると、
後続の整列を含む時間計算量は
$O(\sum_v b_v\log b_v)$、空間計算量は $O(D+\max_v b_v)$。
同じ部分木を再展開する場合があるため、最悪時間計算量は指数的。

## 注意点

- `weight` が有限な `1` 未満、負辺、負または非有限な推定値には
  `std::invalid_argument` を送出する。
- 関数は現在の経路上にある状態を再訪しない。`Hash` と `Equal` は同じ状態を
  一貫して判定しなければならない。
- `Cost{}` がゼロを表し、加算結果と `long double` への変換が表現可能であることを
  前提とする。
- 最適性には非負辺と許容的ヒューリスティックが必要。Weighted IDA* の
  `weight > 1` では近似率を一律には保証しない。
