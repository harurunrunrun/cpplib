---
title: Greedy Maximum Coverage (貪欲最大被覆)
documentation_of: ../../../src/approximate/set_cover/greedy_max_coverage.hpp
---

`approximate::set_cover::greedy_max_coverage` は、高々 `maximum_sets` 個の集合を貪欲に選び、被覆される要素数を増やす。

~~~cpp
MaxCoverageResult greedy_max_coverage(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& sets,
    std::size_t maximum_sets
)
~~~

各反復で未被覆要素を最も多く含む集合を選ぶ。同数なら添字が小さい集合を選ぶ。各集合内の重複要素は 1 個として扱う。選択上限を $K$ とした最大被覆問題に対し、最適値の $1-1/e$ 倍以上を覆う。新たに覆える要素を持つ集合がなくなれば、$K$ 個未満でも終了する。

集合数を $M$、宇宙の大きさを $U$、重複除去後の所属関係の総数を $L$ とする。

- 時間計算量: $O(L\log L + L\log M + U)$
- 追加空間計算量: $O(U+M+L)$

戻り値 `MaxCoverageResult` のメンバ `selected_sets` は選んだ集合の添字（選択順）、`covered_elements` と `uncovered_elements` はそれぞれ被覆された要素・されなかった要素の昇順列である。

## 注意点

- 要素番号は $[0, U)$（$U$ は `universe_size`）になければならない。範囲外の要素があると `std::out_of_range` を投げる。
- `uncovered_elements` が空でないことは、上限 $K$ 個では覆いきれなかったこと（または集合族全体でも覆えないこと）を意味する。
