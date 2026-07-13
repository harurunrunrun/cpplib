---
title: Greedy and add-drop facility location
documentation_of: ../src/approximate/clustering/facility_location.hpp
---

`FacilityLocationResult`は昇順の`facilities`、clientごとのfacility配列内位置
`labels`、開設費と接続費の合計`cost`、採用した局所操作数
`local_search_iterations`を持つ。

## `greedy_facility_location`

```cpp
auto result = greedy_facility_location(F, C, opening_cost, distance);
```

最良単一facilityから始め、目的値を厳密に改善する最良の1-addを反復する。
時間$O(F^3C)$、追加領域$O(FC)$。

## `facility_add_drop_local_search`

```cpp
auto result = facility_add_drop_local_search(
    F, C, opening_cost, distance, maximum_iterations
);
```

貪欲解から最良の1-addまたは1-dropを反復する。採用反復数$I$に対し時間
$O(F^3C+IF^2C)$、追加領域$O(FC)$。同値候補ではfacility添字列の辞書順を使う。

両APIとも`opening_cost[f] + distance(f,c)`を目的値に用いる。費用は有限かつ
非負でなければならない。`F==0`や配列長不一致では`std::invalid_argument`、
`C==0`ではfacilityを開かない費用0の空解を返す。
目的値の加算が`long double`の範囲を超える場合は`std::overflow_error`を送出する。
