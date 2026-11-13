---
title: Metric k-Center / Hierarchical Clustering / Facility Location (距離空間クラスタリング)
documentation_of: ../../../src/approximate/clustering/metric_k_center.hpp
---

距離関数を受け取る k-center 問題のヒューリスティックを `approximate::clustering` 名前空間で提供する。距離は有限かつ非負でなければならない。

## `KCenterResult` / `gonzalez_k_center`

```cpp
auto result = gonzalez_k_center(n, k, distance, first_center = 0);
```

Gonzalez の farthest-first traversal を行う。`centers` は中心を選んだ順の列、`labels[i]` は点 `i` を割り当てた中心の `centers` 内での位置、`radius` は割当距離の最大値である。次の中心は同距離なら小さい点添字を、割当は同距離なら小さい中心位置を選ぶ。距離が距離空間の公理を満たすなら、`radius` は最適な k-center 半径の 2 倍以下になる。

距離関数の評価を $O(1)$ とすると、時間計算量は $O(nk)$、追加空間計算量は $O(n+k)$ である。

## 注意点

- `k == 0` または `k > n` では `std::invalid_argument`、`first_center >= n` では `std::out_of_range` を送出する。距離が負または非有限なら `std::invalid_argument` を送出する。
- 階層的クラスタリング（`agglomerative_clustering`）は `hierarchical_clustering.hpp`、施設配置（`greedy_facility_location` など）は `facility_location.hpp` で提供され、このヘッダには含まれない。
