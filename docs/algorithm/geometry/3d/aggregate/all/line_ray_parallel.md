---
title: 3D Line-Ray Parallelism (三次元直線と半直線の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_ray_parallel.hpp
---

## API

- `parallel(line, ray)`: `Line3` と `Ray3` の方向が平行なら `true` を返す。
- `parallel(ray, line)`: 引数の順序を入れ替えた同じ判定を行う。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 支持直線が一致するかどうかは判定しない。
- 退化した直線または退化した半直線には `std::invalid_argument` を送出する。
