---
title: Routing Distance Matrix Helpers (経路距離行列補助)
documentation_of: ../src/approximate/routing/distance_matrix.hpp
---

## Routing Distance Matrix Helpers (経路距離行列補助)

巡回路heuristicで距離行列と頂点番号を検査する内部API。

## API

### `internal::distance_cost_t<DistanceMatrix>`

`distance[i][j]` のcv・参照を除いた値型を得るalias。compile時処理であり実行時計算量を持たない。

### `internal::validate_distance_matrix(distance)`

正方行列であることを検査し、頂点数を返す。頂点数を $N$ として時間計算量は $O(N)$、追加空間計算量は $O(1)$。

### `internal::validate_vertex(vertex, n)`

頂点が `[0,n)` にあることを検査する。時間・追加空間計算量は $O(1)$。

### `internal::validate_tour_vertices(tour, n)`

巡回列の全頂点を検査する。列長を $L$ として時間計算量は $O(L)$、追加空間計算量は $O(1)$。

## 注意点

すべて `approximate::routing::internal` の内部APIである。非正方行列では `std::invalid_argument`、範囲外頂点では `std::out_of_range` を送出する。距離値自体の有限性・非負性・対称性は検査しない。
