---
title: 3D Tetrahedron Incenter (三次元四面体の内心)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/tetrahedron/tetrahedron_incenter.hpp
---

## API

- `tetrahedron_incenter(tetrahedron)`: 4 面から等距離にある内心を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 退化四面体や非有限入力には `std::invalid_argument` を送出する。退化判定には厳密な `adaptive_orient3d` の符号を用いる。
- 頂点 `a` からの辺の差分を共通のスケールへ正規化し、各面の局所的な面積を重みとして内心を求め、FMA で元の座標へ復元する。
- 局所的な形状または結果を表現できない場合は `std::overflow_error` を送出する。
