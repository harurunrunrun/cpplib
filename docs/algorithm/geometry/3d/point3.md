---
title: 3D Point (三次元点)
documentation_of: ../../../../src/algorithm/geometry/3d/point3.hpp
---

## API

- `Point3{x, y, z}`: 三次元点またはベクトルを構築する。
- 単項符号、加減算、スカラー倍・除算、複合代入を提供する。
- `operator==`: 各座標を絶対誤差付きで比較する。
- `operator<`: 座標を厳密な辞書順で比較する。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域 $O(1)$。

## 注意点

`operator==` と `operator<` の同値関係は一致しない。
