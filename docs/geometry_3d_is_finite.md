---
title: 3D Finite Value Check (三次元幾何の有限値判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/is_finite.hpp
---

## API

- `geometry3d_is_finite(value)`: `long double` が有限なら `true`。
- `geometry3d_is_finite(point)`: `Point3` の全座標が有限なら `true`。
- `geometry3d_is_finite(primitive)`: `Line3`、`Ray3`、`Segment3`、`Plane3`、`Circle3`、`Sphere3`、`Triangle3`、`Tetrahedron3` の全数値要素が有限なら `true`。
- `geometry3d_validate(circle)`: 中心・法線・半径が有限、法線が非零、半径が非負であることを検査する。
- `geometry3d_validate(sphere)`: 中心・半径が有限かつ半径が非負であることを検査する。
- `geometry3d_safe_normalized_difference(left, right, scale)`: 差を安全に正規化する。
- `Geometry3DNormalizedDifference`: 正規化した差 `value` と実座標倍率 `scale` を保持する型。
- `geometry3d_normalized_difference(left, right, additional_extents)`: 2点の差と追加長さを共通スケールで正規化する。
- `geometry3d_scaled_sign(value, scale)`: `GEOMETRY3D_EPS * scale` を許容誤差として符号を返す。
- `geometry3d_compare_nonnegative_scaled(...)`: 倍率付きの非負値同士を overflow / underflow せず比較する。

## API別の時間計算量・空間計算量

上記の全APIは時間・追加領域 $O(1)$。

## 注意点

geometry3d_is_finite は有限性だけを判定し、半径や退化条件は検査しない。geometry3d_validate は契約違反時、差の正規化と相対符号APIは非有限値・非正スケールなどの契約違反時に std::invalid_argument を送出する。NaN と正負の無限大に対する有限値判定は false になる。
