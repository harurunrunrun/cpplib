---
title: 3D Affine Transform (三次元アフィン変換)
documentation_of: ../src/algorithm/geometry/3d/affine_transform3.hpp
---

`AffineTransform3` は点 $p$ を `linear * p + offset` へ移す三次元アフィン変換を
表す。剛体変換に加え、一様・非一様拡大縮小も同じ型で合成できる。

## API

- `AffineTransform3()`, `identity()`: 恒等変換を構築する。
- `AffineTransform3(linear, offset)`: 線形部と平行移動部を直接指定する。
- `translation(displacement)`: 平行移動を構築する。
- `uniform_scaling(factor)`: 原点中心の一様拡大縮小を構築する。
- `nonuniform_scaling(factors)`: 各座標軸方向の原点中心拡大縮小を構築する。
- `rotation_about_origin(axis, theta)`: 原点を通る `axis` 周りの右手系回転を構築する。
- `rotation_about_axis(line, theta)`: 任意の `Line3` 周りの右手系回転を構築する。
- `rotation_about_point(center, axis, theta)`: `center` を通る指定方向軸周りの回転を構築する。
- `reflection_across_plane(plane)`: 平面に関する鏡映を構築する。
- `half_turn_about_line(line)`: 直線周りの180度回転を構築する。
- `apply(point)`, `operator()(point)`: 点へ変換を適用する。
- `outer.compose(inner)`, `outer * inner`: `outer(inner(point))` と等しい合成を返す。
- `inverse(relative_epsilon)`: 逆変換を返す。線形部が数値的に特異なら `nullopt`。

## API別の時間計算量・空間計算量

各構築、点への適用、合成、逆変換はいずれも固定サイズ行列だけを扱うため、
時間・追加領域は $O(1)$。

## 注意点

- 回転角はラジアンであり、右手系を用いる。
- 回転軸の方向が零、`Line3` が退化、または平面法線が零なら
  `std::invalid_argument` を送出する。
- factorに0を含む拡大縮小は構築できるが、逆変換は存在しない。
- 非一様拡大縮小を含む一般の `AffineTransform3` は剛体変換ではない。
- 入力と中間値が有限な `long double` の範囲に収まる必要がある。
