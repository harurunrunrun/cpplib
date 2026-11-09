---
title: Lattice Polygon Interior Points (格子多角形の内部格子点数)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/scalar/lattice_polygon_interior_points.hpp
---

格子多角形の内部（境界を除く）にある格子点の個数を求める。

## API

```cpp
template<std::integral Coordinate>
__uint128_t lattice_polygon_interior_points(
    const std::vector<IntegerPoint<Coordinate>>& polygon
);
```

## 引数

3 頂点以上の単純な格子多角形の頂点を周回順に並べた列を渡す。

## 戻り値

Pick の定理から求めた内部の格子点の個数。面積を $S$、境界上の格子点数を $B$ とすると、内部の格子点数は $I = S - B / 2 + 1$ である。

## 時間計算量

頂点数を $N$、座標差の最大絶対値を $C$ とすると、時間計算量は $O(N \log C)$、追加領域は $O(1)$ である。

## 注意点

- 頂点数が 3 未満、面積が 0、または Pick の定理と整合しない入力（$2S - B + 2$ が負または奇数）では `std::invalid_argument` を送出する。
- 結果が `__uint128_t` に収まらない場合は `std::overflow_error` を送出する。
