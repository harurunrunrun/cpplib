---
title: 2D Incenter (二次元内心)
documentation_of: ../src/algorithm/geometry/2d/point/incenter.hpp
---

## API

- `incenter(first, second, third)`: 三角形の内心を返す。

## API別の時間計算量・空間計算量

- `incenter`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化三角形には `std::invalid_argument` を送出し、退化判定は2辺の角度基準で行う。
辺長には `hypot` 相当を用い、辺長weightを正規化して最初の頂点からの差分を加えるため、絶対座標と辺長の積を中間計算しない。
