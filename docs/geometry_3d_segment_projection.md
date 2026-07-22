---
title: 3D Segment Projection (三次元線分への射影)
documentation_of: ../src/algorithm/geometry/3d/point/segment_projection.hpp
---

## API

- `projection(segment, point)`: 閉線分上で点に最も近い射影点を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化線分では唯一の端点を返す。直交射影のparameterは $[0,1]$ にclampされる。
