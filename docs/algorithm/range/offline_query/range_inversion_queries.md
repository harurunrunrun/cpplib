---
title: Offline Range Inversion Queries (区間転倒数のオフライン一括照会) [static_range_inversions_query]
documentation_of: ../../../../src/algorithm/range/offline_query/range_inversion_queries.hpp
---

`range_inversion_queries(values, ranges)` は各半開区間の転倒数を入力順の `vector<long long>` で返す。
転倒は `i<j` かつ `values[i]>values[j]`。同値の対は数えない。負数や重複、空区間も許す。
Mo の区間走査中、圧縮値上の Fenwick 木で左右端の寄与を更新する。

## 時間計算量

$Q>0$ で $O(N\log N+Q\log Q+(N\sqrt Q+Q)\log(N+1))$ 時間、$O(N+Q)$ 空間。
`Q=0` のとき $O(N\log N)$ 時間。

## 注意点

各範囲は `0 <= l <= r <= N`。違反時は `out_of_range`。
全照会を事前に受け取る。転倒数が `long long` に収まる必要がある。
