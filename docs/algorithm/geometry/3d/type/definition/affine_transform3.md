---
title: 3D Affine Transform (三次元アフィン変換)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/affine_transform3.hpp
---

`AffineTransform3` は点 $p$ を $Ap + b$ （$A$ は `linear`、$b$ は `offset`）へ移す三次元アフィン変換を表す。剛体変換に加え、一様・非一様な拡大縮小も同じ型で合成できる。

## API

- `AffineTransform3()`, `identity()`: 恒等変換を構築する。
- `AffineTransform3(linear, offset)`: 線形部（`Matrix3`）と平行移動部（`Point3`）を直接指定する。
- `translation(displacement)`: 平行移動を構築する。
- `uniform_scaling(factor)`: 原点中心の一様な拡大縮小を構築する。
- `nonuniform_scaling(factors)`: 原点中心に、各座標軸方向へ `factors.x`, `factors.y`, `factors.z` 倍する拡大縮小を構築する。
- `rotation_about_origin(axis, theta)`: 原点を通り方向 `axis` の軸周りの、右手系での回転を構築する。
- `rotation_about_axis(line, theta)`: 任意の `Line3` 周りの右手系での回転を構築する。
- `rotation_about_point(center, axis, theta)`: `center` を通り方向 `axis` の軸周りの回転を構築する。
- `reflection_across_plane(plane)`: 平面に関する鏡映を構築する。
- `half_turn_about_line(line)`: 直線周りの 180 度回転を構築する。
- `apply(point)`, `operator()(point)`: 点に変換を適用する。
- `outer.compose(inner)`, `outer * inner`: 任意の点 `point` に対して `outer(inner(point))` と等しい合成変換を返す。
- `inverse(relative_epsilon = GEOMETRY3D_EPS)`: 逆変換を `std::optional<AffineTransform3>` で返す。線形部が数値的に特異なら `std::nullopt` を返す。

## API別の時間計算量・空間計算量

各構築、点への適用、合成、逆変換はいずれも固定サイズの行列だけを扱うため、時間・追加領域は $O(1)$。

## 注意点

- 回転角はラジアンで指定し、右手系を用いる。
- 回転軸の方向が零ベクトルの場合、`Line3` が退化している場合、平面の法線が零ベクトルの場合は `std::invalid_argument` を送出する。
- 倍率に $0$ を含む拡大縮小も構築できるが、その逆変換は存在しない。
- 非一様な拡大縮小を含む一般の `AffineTransform3` は剛体変換ではない。
- 入力と中間値が有限な `long double` の範囲に収まる必要がある。
