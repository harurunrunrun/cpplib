---
title: Range Linear Add Minimum (添字一次式加算と区間最小値) [range_linear_add_range_min]
documentation_of: ../../../../src/structure/range_query/extrema/range_linear_add_min.hpp
---

`RangeLinearAddMin(vector<int64_t> values)` は配列を平方根個の block に分ける。
`add(l,r,b,c)` は各 `i` in `[l,r)` へ `b*i+c` を加える。
`minimum(l,r)` は非空半開区間の最小値。`size()` は配列長。
`minimum_with_count(l,r)` は `(最小値,その出現回数)` を返す。
block 内の下側凸包と一次式の遅延作用を使い、全域更新も線形走査しない。

## 時間計算量

構築 $O(N)$、更新 $O(\sqrt N)$、照会 $O(\sqrt N\log(N+1))$、保存空間 $O(N)$。
出現回数付きの照会も同じ計算量。凸包の辺上にある点を残し、最小値を共有する plateau を二分探索する。

## 注意点

区間は `0<=l<=r<=N`。最小値の空区間は `invalid_argument`、範囲外は `out_of_range`。
値、遅延係数、`b*i+c` と全更新結果は `int64_t` に収まる必要がある。凸包の積の比較には GCC 13 の `__int128` を使う。
