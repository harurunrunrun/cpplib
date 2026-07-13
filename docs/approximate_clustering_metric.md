---
title: Metric k-center / Hierarchical clustering / Facility location
documentation_of: ../src/approximate/clustering/metric_k_center.hpp
---

距離callableを受け取るクラスタリングとfacility locationのヒューリスティックを
`approximate::clustering`名前空間で提供する。距離と開設費は有限かつ非負で
なければならない。

## `KCenterResult` / `gonzalez_k_center`

```cpp
auto result = gonzalez_k_center(n, k, distance, first_center);
```

Gonzalezのfarthest-first traversalを行う。`centers`は選択順、`labels[i]`は
選択列内の割当位置、`radius`は最大割当距離。同距離の次中心は小さい点添字、
割当は小さい中心添字を選ぶ。距離がmetricなら最適k-center半径の2倍以下になる。
時間$O(nk)$、追加領域$O(n+k)$（距離callableを$O(1)$とする）。

## `Linkage` / `HierarchicalMerge`

`Linkage`は`single`、`complete`、`average`を持つ。
`HierarchicalMerge{left,right,distance,size}`は2 clusterの結合と結合後サイズを表す。
葉は`0..n-1`、`merges[i]`が作るclusterは`n+i`である。

## `agglomerative_clustering`

```cpp
auto result = agglomerative_clustering(n, distance, linkage);
```

最小linkage距離の2 clusterを反復結合する。同距離ではcluster IDの辞書順を使う。
`HierarchicalClusteringResult::merges`に$n-1$回の結合を格納する。距離表と
Lance--Williams更新を用い、時間$O(n^3)$、追加領域$O(n^2)$。

## `FacilityLocationResult`

- `facilities`: 昇順の開設facility添字。
- `labels[c]`: client `c`を割り当てた`facilities`内の位置。
- `cost`: 開設費と全client接続費の和。
- `local_search_iterations`: add/drop局所探索で採用した操作数。

## `greedy_facility_location`

```cpp
auto result = greedy_facility_location(facility_count, client_count,
                                       opening_cost, distance);
```

最良の単一facilityから始め、目的値を改善するfacilityを1個ずつ追加する。
facility数$F$、client数$C$に対し、時間$O(F^3C)$、追加領域$O(FC)$。

## `facility_add_drop_local_search`

```cpp
auto result = facility_add_drop_local_search(
    facility_count, client_count, opening_cost, distance, maximum_iterations
);
```

貪欲解から最良の1-addまたは1-dropを反復する。採用反復数を$I$とすると時間
$O(F^3C+IF^2C)$、追加領域$O(FC)$。clientが0なら空解（費用0）を返す。

`k==0`、`k>n`、範囲外の`first_center`、開設費配列長の不一致などは
`std::invalid_argument`または`std::out_of_range`を送出する。
