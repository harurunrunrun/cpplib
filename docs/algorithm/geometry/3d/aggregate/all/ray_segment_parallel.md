---
title: 3D Ray-Segment Parallelism (三次元半直線と線分の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/ray_segment_parallel.hpp
---

## API

- `parallel(ray, segment)`: `Ray3` と `Segment3` の方向ベクトルが平行なら `true` を返す。
- `parallel(segment, ray)`: 引数の順を逆にした同じ判定を行う。

## API別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加領域ともに $O(1)$。

## 注意点

方向だけを比べ、重なっているかどうかは判定しない。退化した半直線または退化した線分には `std::invalid_argument` を送出する。
