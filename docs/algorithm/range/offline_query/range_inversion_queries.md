---
title: Offline Range Inversion Queries (区間転倒数のオフライン一括照会) [static_range_inversions_query]
documentation_of: ../../../../src/algorithm/range/offline_query/range_inversion_queries.hpp
---

各半開区間の転倒数をまとめて求める。

## API

```cpp
template<class T>
vector<long long> range_inversion_queries(
    const vector<T>& values, const vector<pair<int, int>>& ranges);
```

各半開区間 $[l, r)$ の転倒数を入力順に返す。転倒は $i<j$ かつ `values[i] > values[j]` を満たす組であり、等しい値の組は数えない。負の値、重複、空区間も許す。Mo 法で区間を走査しながら、座標圧縮した値の上の Fenwick 木で左右端の寄与を更新する。

## 時間計算量

$Q>0$ で $O(N\log N+Q\log Q+(N\sqrt Q+Q)\log(N+1))$ 時間、$O(N+Q)$ 空間。$Q=0$ のときは $O(N\log N)$ 時間。

## 注意点

- 各範囲は `0 <= l <= r <= N` でなければならない。違反時は `out_of_range` を送出する。
- すべての問い合わせを事前に受け取るオフライン処理である。
- 転倒数が `long long` に収まらなければならない。
