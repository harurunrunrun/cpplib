---
title: Weighted Point (重み付き点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/weighted_point.hpp
---

幾何的なサイトの座標 `point` と、加法重みまたは二乗距離重み `weight` を格納する。

## API

```cpp
struct WeightedPoint {
    Point point;
    long double weight;
};
```

`WeightedPoint{point, weight}` で構築し、2 つの公開メンバを参照する。

## 時間計算量

構築とメンバの参照は時間 $O(1)$、空間 $O(1)$。

## 注意点

重みの解釈は、この型を利用する図（Voronoi 図の変種など）の API によって異なる。
`weight` にはデフォルトメンバ初期化子がないため、デフォルト初期化では値が不定になる。
