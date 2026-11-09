---
title: Offline Range Mode Queries (区間最頻値のオフライン一括照会) [static_range_mode_query]
documentation_of: ../../../../src/algorithm/range/offline_query/range_mode_queries.hpp
---

`range_mode_queries(values, ranges)` は、各非空半開区間 `[l, r)` について `(最頻値, 出現回数)` を入力順に返す。
Mo 法による区間走査と、出現回数ごとの双方向リストを用いる。

## 使い方

```cpp
template<class T>
vector<pair<T, int>> range_mode_queries(
    const vector<T>& values,
    const vector<pair<int, int>>& ranges
)
```

- 値型 `T` は `operator<` による比較、`operator==` による等値比較、コピーが可能であること（内部で座標圧縮する）。
- 負の値や重複する値を許す。
- 最頻値が複数ある場合は、そのうち任意の 1 つを返す。

## 時間計算量

配列長を $N$、照会数を $Q$ とすると、$Q>0$ のとき $O(N\log N+Q\log Q+N\sqrt Q+Q)$ 時間である。
$Q=0$ のときは $O(N\log N)$ 時間である。

## 空間計算量

$O(N+Q)$。

## 注意点

- 各区間は `0 <= l < r <= N` を満たす必要があり、違反時は `std::out_of_range` を送出する。
- 全照会を事前に受け取るオフライン処理である。
- 同率の最頻値のうちどれを返すかに依存しないこと。
