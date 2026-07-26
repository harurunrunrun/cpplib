---
title: Principal Axes 3D Result (三次元主成分軸結果型)
documentation_of: ../../../../src/algorithm/geometry/3d/principal_axes3.hpp
---

## API

- `PrincipalAxes3::centroid`: 入力点集合の重心。
- `PrincipalAxes3::axes`: 対応する分散の降順に並ぶ3本の主成分軸。
- `PrincipalAxes3::variances`: 各軸方向の母分散。

## API別の時間計算量・空間計算量

- 構築・各fieldへのアクセス: 時間・追加領域 $O(1)$。

## 注意点

型自体は軸の正規直交性や分散順を検査しない。`principal_axes_3d` が返す各軸は単位ベクトルである。
