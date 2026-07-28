---
title: Weighted Point (重み付き点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/weighted_point.hpp
---

幾何siteの座標 `point` と加法または二乗距離重み `weight` を格納する。

## API

`WeightedPoint{point, weight}` で構築し、2つの公開memberを参照する。

## 時間計算量

構築とmember参照は $O(1)$、空間 $O(1)$。

## 注意点

重みの解釈は利用するdiagram APIによって異なる。
