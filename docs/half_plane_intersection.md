---
title: Half-Plane Intersection (半平面交差)
documentation_of: ../src/algorithm/geometry/2d/advanced/half_plane_intersection.hpp
---

## API

- `half_plane_intersection(half_planes)`: 各有向直線の左閉半平面の共通部分を `HalfPlaneIntersectionResult` で返す。

空、非有界、有界を分類し、有界なら反時計回りの正規化済み頂点列を返す。点・線分への退化も有界に含む。

## API別の時間計算量・空間計算量

- 半平面数を $K$ として、時間 $O(K\log K)$、追加領域 $O(K)$。

## 注意点

退化境界には `std::invalid_argument`、有限な一時境界を構成できない座標には `std::overflow_error` を送出する。空の入力は全平面なので非有界である。
