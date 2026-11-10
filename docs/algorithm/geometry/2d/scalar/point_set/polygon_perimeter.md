---
title: 2D Polygon Perimeter (二次元多角形の周長)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/polygon_perimeter.hpp
---

多角形の周長を求める。

## API

```cpp
long double polygon_perimeter(const std::vector<Point>& polygon);
```

## 引数

周回順に並べた頂点列を渡す。

## 戻り値

隣接する頂点間の距離と、末尾と先頭の間の距離の総和。

## 時間計算量

頂点数を $N$ として、時間計算量 $O(N)$、追加領域 $O(1)$。

## 注意点

頂点数が 0 または 1 なら `0` を返す。頂点数が 2 なら、その線分の長さの 2 倍となる。
