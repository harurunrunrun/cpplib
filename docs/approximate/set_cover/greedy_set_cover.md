---
title: Greedy Set Cover (貪欲集合被覆)
documentation_of: ../../../src/approximate/set_cover/greedy_set_cover.hpp
---

集合族から、未被覆要素を最も効率よく覆う集合を繰り返し選ぶ。名前空間 `approximate::set_cover` にある。各集合内の重複要素は 1 個として扱う。

## greedy_set_cover

~~~cpp
SetCoverResult greedy_set_cover(std::size_t universe_size, const std::vector<std::vector<std::size_t>>& sets)
~~~

毎回、未被覆要素を最も多く含む集合を選ぶ。同数なら添字が小さい集合を選ぶ。被覆可能な要素について、最適解の $H_U$ 倍以下の集合数になる。被覆不能な要素は `uncovered_elements` に昇順で返す。

宇宙の大きさを $U$（`universe_size`）、集合数を $M$、重複除去後の所属関係の総数を $L$ とする。

- 時間計算量: $O(L\log L + L\log M + U)$
- 追加空間計算量: $O(U+M+L)$

## greedy_weighted_set_cover

~~~cpp
template<class Weight>
SetCoverResult greedy_weighted_set_cover(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& sets,
    const std::vector<Weight>& weights
)
~~~

（重み）/（新たに覆う要素数）が最小の集合を選ぶ。同率なら添字が小さい集合を選ぶ。64 bit 以下の整数重みでは比を 128 bit 整数のたすき掛けで厳密に比較し、それ以外（浮動小数点数など）は `long double` での除算で比較する。全要素が被覆可能なら、総重みは最適値の $H_U$ 倍以下になる。

- 時間計算量: $O(L\log L + L\log M + U)$
- 追加空間計算量: $O(U+M+L)$

## SetCoverResult

`selected_sets` は選んだ集合の添字（選択順）、`uncovered_elements` は被覆できなかった要素の昇順列である。`covers_universe()` は `uncovered_elements` が空かどうかを返す。時間計算量 $O(1)$、追加空間計算量 $O(1)$。

## 注意点

- `Weight` は `bool` 以外の算術型とする。重みは非負で、浮動小数点数なら有限でなければならない。
- 重みの個数が集合数と異なる場合や不正な重みには `std::invalid_argument`、宇宙外の要素には `std::out_of_range` を投げる。
- $H_U$ は $U$ 番目の調和数 $1+1/2+\cdots+1/U$ である。
