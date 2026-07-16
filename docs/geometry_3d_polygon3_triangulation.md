---
title: Polygon3 Triangulation (三次元多角形の三角形分割)
documentation_of: ../src/algorithm/geometry/3d/polygon3_triangulation.hpp
---

## API

- `polygon3_triangulation_indices(polygon)`: 各三角形を元頂点indexの3要素配列で返す。
- `polygon3_triangulation(polygon)`: 分割後の`Triangle3`列を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として各APIは時間 $O(N^2)$、追加領域 $O(N)$。平面への射影は
  $O(N)$、単純性検査と耳切り法は合わせて $O(N^2)$ である。

## 注意点

自己交差せず面積が正の共面多角形を与える。不正入力には`std::invalid_argument`を送出する。
