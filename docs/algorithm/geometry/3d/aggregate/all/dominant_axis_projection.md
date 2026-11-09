---
title: Dominant Axis Projection 3D Umbrella (三次元支配軸射影集約)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/dominant_axis_projection.hpp
---

このヘッダは結果型・軸の選択・射影の 3 つの個別ヘッダをまとめて include する、後方互換のための集約ヘッダである。必要な機能だけを使う場合は `dominant_axis3.hpp`、`dominant_axis_from_point3.hpp`、`dominant_axis_projection_to_point.hpp` のいずれかを直接 include すればよい。

## API

- `enum class DominantAxis3 { x, y, z }`: 除去する軸を表す。
- `dominant_axis(normal)`: 法線の成分のうち絶対値が最大のものに対応する軸を返す。同値の場合は $x$, $y$, $z$ の順に優先する。零ベクトル（`geometry3d_sign` で 0 と判定される長さ）なら `std::invalid_argument` を送出する。
- `dominant_axis_projection(point, axis)`: 指定した軸を除去して二次元の `Point` に射影する。`x` を除くと $(y, z)$、`y` を除くと $(z, x)$、`z` を除くと $(x, y)$ を返す（巡回順なので、除去した軸の正の向きから見た向きが保たれる）。

## API 別の時間計算量・空間計算量

- 全 API: 時間・追加空間ともに $O(1)$。
