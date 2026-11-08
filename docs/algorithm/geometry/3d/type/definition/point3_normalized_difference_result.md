---
title: Normalized 3D Difference Result (三次元正規化差分結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/point3_normalized_difference_result.hpp
---

## API

- `Geometry3DNormalizedDifference::value`: 三次元の差分ベクトルを尺度で割ったもの（`Point3`）。
- `Geometry3DNormalizedDifference::scale`: 元の差分へ戻すための正の尺度（`long double`）。元の差分は `value` に `scale` を掛けたものである。

## API別の時間計算量・空間計算量

- 構築・各メンバの参照: 時間・追加領域ともに $O(1)$。

## 注意点

構造体自体は値の整合性を検査しない。通常は `geometry3d_normalized_difference` の戻り値として使う。
