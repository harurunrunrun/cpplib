---
title: 2D Circle Intersection Area (二次元円板の共通部分面積) [CGL_7_I]
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/circle_circle/circle_circle_intersection_area.hpp
---

2 つの円板の共通部分の面積を求める。

## API

```cpp
long double circle_circle_intersection_area(
    const Circle& first,
    const Circle& second
);
```

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

- 2 円が離れている（または外接する）場合と、一方の半径が 0 の場合は 0 を返す。一方が他方を含む場合は、小さい方の円板の面積を返す。
- 弓形の中心角が小さい場合は級数展開で桁落ちを抑えて計算する。
- 円の中心座標・半径が有限でない場合や半径が負の場合は `std::invalid_argument` を送出する。
