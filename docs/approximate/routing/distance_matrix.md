---
title: Routing Distance Matrix Helpers (経路距離行列補助)
documentation_of: ../../../src/approximate/routing/distance_matrix.hpp
---

巡回路ヒューリスティックで、距離行列と頂点番号を検査する内部 API である。

## API

### `internal::distance_cost_t<DistanceMatrix>`

`distance[i][j]` の型から cv 修飾と参照を除いた値型を得る別名。コンパイル時の処理であり、実行時の計算量はない。

### `internal::validate_distance_matrix(distance)`

正方行列であることを検査し、頂点数を返す。頂点数を $N$ として時間計算量は $O(N)$、追加空間計算量は $O(1)$。

### `internal::validate_vertex(vertex, n)`

頂点番号が $[0, n)$ にあることを検査する。時間計算量・追加空間計算量は $O(1)$。

### `internal::validate_tour_vertices(tour, n)`

巡回列のすべての頂点番号を検査する。列の長さを $L$ として時間計算量は $O(L)$、追加空間計算量は $O(1)$。

## 注意点

- すべて `approximate::routing::internal` 名前空間の内部 API である。
- 正方でない行列には `std::invalid_argument` を、範囲外の頂点には `std::out_of_range` を送出する。
- 距離の値そのものの有限性・非負性・対称性は検査しない。
