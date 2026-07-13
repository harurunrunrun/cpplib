---
title: Advanced Geometry Result Types (高度幾何の結果型)
documentation_of: ../src/algorithm/geometry/2d/advanced/base.hpp
---

半平面交差の結果型。

## API

- `HalfPlaneIntersectionKind`: `HALF_PLANE_EMPTY`, `HALF_PLANE_BOUNDED`, `HALF_PLANE_UNBOUNDED` を持つ。
- `HalfPlaneIntersectionResult`: 分類 `kind` と、有界時の頂点列 `polygon` を持つ。

## API別の時間計算量・空間計算量

- enum参照と空の結果構築: 時間・追加領域 $O(1)$。
- `polygon` のcopy: 頂点数を $H$ として時間・領域 $O(H)$。

## 注意点

非有界または空の結果では `polygon` は空である。このheaderは高度凸幾何APIの共通結果型を定義する。
