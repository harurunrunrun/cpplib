---
title: Polygon3 Point Distance (三次元多角形と点の距離)
documentation_of: ../src/algorithm/geometry/3d/polygon3_distance.hpp
---

## API

- `polygon3_distance(polygon, point)`: 閉多角形領域と点の最短距離を返す。
  凹多角形でも、面への射影が領域外なら全境界辺との最短距離を返す。

## API別の時間計算量・空間計算量

- `polygon3_distance(polygon, point)`: 頂点数を $N$ として時間 $O(N)$、
  追加領域 $O(N)$。

## 注意点

- `polygon` は頂点数3以上の単純な共面多角形で、面積が正でなければならない。
  頂点数不足、面積0、非共面の場合は `std::invalid_argument` を送出する。
- 全頂点と `point` は有限でなければならず、違反時は
  `std::invalid_argument` を送出する。
- 座標差と多角形の局所scaleを正規化して計算するため、非常に大きい・小さい
  一様scaleと巨大な共通平行移動による中間overflow・underflowを抑える。
- 真の距離が `long double` で表現できない場合は `std::overflow_error` を送出する。
- 自己交差の検査は行わない。
