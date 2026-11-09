---
title: Point on 3D Segment (三次元線分上判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_point/on_segment.hpp
---

## API

- `on_segment(segment, point)`: 点 `point` が端点を含む閉線分 `segment` 上にあるかを判定する。

## API別の時間計算量・空間計算量

- `on_segment`: 時間・追加領域ともに $O(1)$。

## 注意点

- 退化した線分は 1 点として扱い、その端点と一致する場合だけ `true` を返す。
- 端点との比較には `Point3` の `==`（各座標の差が `GEOMETRY3D_EPS` 以下という絶対許容誤差）を用いるため、計算で端点へ丸め戻された点も線分上と判定する。
- それ以外の点は、方向の平行判定と内積の符号判定（いずれも許容誤差付き）で判定する。
