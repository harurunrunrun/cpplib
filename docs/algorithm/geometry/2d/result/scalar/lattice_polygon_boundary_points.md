---
title: Lattice Polygon Boundary Points (格子多角形の境界格子点数)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/scalar/lattice_polygon_boundary_points.hpp
---

格子多角形の境界上にある格子点の個数を求める。

## API

```cpp
template<std::integral Coordinate>
__uint128_t lattice_polygon_boundary_points(
    const std::vector<IntegerPoint<Coordinate>>& polygon
);
```

## 引数

整数座標の多角形の頂点を周回順に並べた列を渡す。

## 戻り値

境界上にある相異なる格子点の個数。各辺 $(p_i, p_{i+1})$ について $\gcd(\lvert x_{i+1} - x_i \rvert, \lvert y_{i+1} - y_i \rvert)$ を足し合わせた値である。

## 時間計算量

頂点数を $N$、座標差の最大絶対値を $C$ とすると、時間計算量は $O(N \log C)$、追加領域は $O(1)$ である。

## 注意点

座標差は符号なし整数で計算するため、`long long` の両端の値どうしの差でもオーバーフローしない。単純多角形であることを前提とする。
