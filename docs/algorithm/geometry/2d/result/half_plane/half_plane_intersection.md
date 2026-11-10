---
title: Half-Plane Intersection (半平面交差)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/half_plane/half_plane_intersection.hpp
---

## API

- `HalfPlaneIntersectionResult half_plane_intersection(std::vector<Line> half_planes)`: 各有向直線 `a -> b` の左側の閉半平面の共通部分を求める。

結果の `kind` は `HALF_PLANE_EMPTY`（空）、`HALF_PLANE_UNBOUNDED`（非有界）、`HALF_PLANE_BOUNDED`（有界）のいずれかである。有界のときだけ、`polygon` に `normalize_convex_polygon` で正規化した反時計回りの頂点列が入る（空・非有界のときは空列）。点や線分に退化した共通部分も有界として扱う。

## API別の時間計算量・空間計算量

- `half_plane_intersection`: 半平面の個数を $K$ として、時間 $O(K \log K)$、追加領域 $O(K)$。

## 注意点

- 境界の 2 点の距離が `GEOMETRY_EPS` 以下の退化境界、または非有限な座標には `std::invalid_argument` を送出する。
- 内部で入力の座標に応じた大きさの補助境界（バウンディングボックス）を追加して非有界性を判定する。補助境界を有限に構成できないほど座標が大きい場合は `std::overflow_error` を送出する。
- 空の入力は平面全体を表すので、非有界と判定する。
