---
title: Greedy Maximum Coverage (貪欲最大被覆)
documentation_of: ../src/approximate/set_cover/greedy_max_coverage.hpp
---

`approximate::set_cover::greedy_max_coverage` は、最大 `maximum_sets` 個の集合を選び、被覆要素数を貪欲に増やす。

~~~cpp
MaxCoverageResult greedy_max_coverage(
    size_t universe_size,
    const vector<vector<size_t>>& sets,
    size_t maximum_sets
)
~~~

各反復で未被覆要素を最も多く含む集合を選ぶ。同数なら添字が小さい集合を選ぶ。選択上限を $K$ とした最大被覆問題に対し、最適値の $1-1/e$ 倍以上を覆う。正の増分を持つ集合がなくなれば $K$ 個未満でも終了する。

集合数を $M$、宇宙の大きさを $U$、重複除去後の所属関係の総数を $L$ とする。

- 時間計算量: $O(L\log L + L\log M + U)$
- 追加空間計算量: $O(U+M+L)$

`selected_sets` は選択順、`covered_elements` と `uncovered_elements` は要素の昇順列である。宇宙外の要素には `std::out_of_range` を送出する。

## 注意点

要素番号は宇宙の範囲内で、重みは各APIの型・有限性・符号条件を満たす必要がある。返却結果に未被覆要素が残る場合、指定された集合族だけでは全被覆できない。
