---
title: Greedy Set Cover (貪欲集合被覆)
documentation_of: ../src/approximate/set_cover/greedy_set_cover.hpp
---

集合族から未被覆要素を最も効率よく覆う集合を反復して選ぶ。`approximate::set_cover` 名前空間にある。各集合内の重複要素は1個として扱う。

# greedy_set_cover

~~~cpp
SetCoverResult greedy_set_cover(size_t universe_size, const vector<vector<size_t>>& sets)
~~~

毎回、未被覆要素を最も多く含む集合を選ぶ。同数なら添字が小さい集合を選ぶ。被覆可能な要素について、最適解の $H_U$ 倍以下の集合数になる。被覆不能な要素は `uncovered_elements` に昇順で返す。

集合数を $M$、重複除去後の所属関係の総数を $L$ とする。

- 時間計算量: $O(L\log L + L\log M + U)$
- 追加空間計算量: $O(U+M+L)$

# greedy_weighted_set_cover

~~~cpp
template<class Weight>
SetCoverResult greedy_weighted_set_cover(
    size_t universe_size,
    const vector<vector<size_t>>& sets,
    const vector<Weight>& weights
)
~~~

`weight / 新たに覆う要素数` が最小の集合を選ぶ。同率なら添字が小さい集合を選ぶ。整数重みの比は128-bit整数の交差積、浮動小数点重みは正の要素数による除算で比較する。被覆可能かつ非負重みの場合、最適値の $H_U$ 倍以下になる。

- 時間計算量: $O(L\log L + L\log M + U)$
- 追加空間計算量: $O(U+M+L)$

# SetCoverResult

`selected_sets` は選択順、`uncovered_elements` は被覆不能だった要素の昇順列である。`covers_universe()` は後者が空かを返し、時間計算量 $O(1)$、追加空間計算量は $O(1)$。

## 注意点

`Weight` は `bool` 以外の算術型とする。重みは非負、浮動小数点数では有限でなければならない。重み数の不一致・不正な重みには `std::invalid_argument`、宇宙外の要素には `std::out_of_range` を送出する。
