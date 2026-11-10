---
title: 2D Incenter (二次元内心)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/point_point_point/incenter.hpp
---

## API

- `Point incenter(first, second, third)`: 三角形の内心を返す。

## API別の時間計算量・空間計算量

- `incenter`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化三角形には `std::invalid_argument` を送出する。退化の判定は、2 辺の単位方向ベクトルの外積（角度）を基準に行う。
辺の長さは `hypot` で求め、重みとなる辺の長さを最大辺で正規化してから最初の頂点からの差分に掛けて足すので、絶対座標と辺の長さの積を中間計算することはない。
