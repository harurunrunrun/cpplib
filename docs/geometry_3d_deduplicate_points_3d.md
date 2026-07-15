---
title: 3D Point Deduplication (三次元点の重複除去)
documentation_of: ../src/algorithm/geometry/3d/deduplicate_points_3d.hpp
---

## API

```cpp
vector<Point3> deduplicate_points_3d(points, relative_tolerance, absolute_tolerance)
```

各座標を `relative_equal` で比較し、先に現れた代表点を残すgreedyな重複除去を行う。既定値は相対許容誤差0、絶対許容誤差 `GEOMETRY3D_EPS`。

## API別の時間計算量・空間計算量

入力点数を $N$、残る点数を $U$ とすると時間 $O(NU)$、戻り値を含む追加領域 $O(U)$。

## 注意点

全座標は有限、許容誤差は有限かつ非負でなければならず、違反時は `std::invalid_argument`。誤差付き等価関係は推移的とは限らないため、結果は入力順に依存する。Delaunayなどで組合せ構造を作る場合は、必要に応じて許容誤差0で正確な座標重複だけを除く。
