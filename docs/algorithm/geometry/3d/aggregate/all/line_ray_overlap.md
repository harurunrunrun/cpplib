---
title: 3D Line-Ray Overlap (三次元直線と半直線の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_ray_overlap.hpp
---

## API

- `overlap(line, ray)`: `Line3` と `Ray3` の共通部分が正の長さを持つ（共通部分が半直線になる）なら `true` を返す。
- `overlap(ray, line)`: 引数の順序を入れ替えた同じ判定を行う。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 1 点だけで接する場合は `false` を返す。
- 退化した直線または退化した半直線には `std::invalid_argument` を送出する。
