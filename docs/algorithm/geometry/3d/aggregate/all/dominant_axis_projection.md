---
title: Dominant Axis Projection 3D Umbrella (三次元支配軸射影集約)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/dominant_axis_projection.hpp
---

## API

- `DominantAxis3`: 除去対象の軸を表す。
- `dominant_axis(normal)`: 絶対値最大の法線成分に対応する軸を返す。
- `dominant_axis_projection(point, axis)`: 指定軸を除去して `Point` へ射影する。
- このヘッダは結果型・軸選択・射影の3 leafをまとめてincludeする後方互換aggregatorである。

## API別の時間計算量・空間計算量

- 全API: 時間・追加領域 $O(1)$。

## 注意点

必要な責務だけを使う場合は `dominant_axis3.hpp`、`dominant_axis_from_point3.hpp`、`dominant_axis_projection_to_point.hpp` のいずれかを直接includeできる。
