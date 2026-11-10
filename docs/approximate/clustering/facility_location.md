---
title: Greedy and Add-Drop Facility Location (貪欲法と追加削除法による施設配置)
documentation_of: ../../../src/approximate/clustering/facility_location.hpp
---

戻り値の `FacilityLocationResult` は、開設した施設の昇順列 `facilities`、各顧客を割り当てた施設の `facilities` 内での位置 `labels`、開設費用と接続コストの合計 `cost`、採用した局所操作の回数 `local_search_iterations` を持つ。

## `greedy_facility_location`

```cpp
auto result = greedy_facility_location(F, C, opening_cost, distance);
```

最良の単一施設から始め、目的値を厳密に改善する最良の 1 施設追加を繰り返す。時間計算量は $O(F^3C)$、追加空間計算量は $O(FC)$ である。

## `facility_add_drop_local_search`

```cpp
auto result = facility_add_drop_local_search(
    F, C, opening_cost, distance, maximum_iterations = 100
);
```

貪欲解から始め、最良の 1 施設追加または 1 施設削除を繰り返す。採用した反復回数を $I$ とすると、時間計算量は $O(F^3C+IF^2C)$、追加空間計算量は $O(FC)$ である。同値の候補は施設添字列の辞書順で比較する。

## 注意点

- どちらの API も、開設した施設の `opening_cost[f]` の和と、各顧客 `c` の `distance(f, c)` の最小値の和を目的値とする。
- 費用は有限かつ非負でなければならない。`F == 0` や配列長の不一致では `std::invalid_argument` を送出する。`C == 0` では施設を開かない費用 0 の空解を返す。
- 目的値の加算が `long double` の範囲を超える場合は `std::overflow_error` を送出する。
- 局所探索であり、大域最適性は保証しない。
