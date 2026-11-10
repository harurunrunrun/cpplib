---
title: Rectangle Union Area (長方形の和集合面積) [area_of_union_of_rectangles]
documentation_of: ../../../../../src/algorithm/other/offline_query/grid_aggregation/rectangle_union.hpp
---

軸平行な長方形の和集合の面積を求める。

## 関数

```cpp
template<class T>
T rectangle_union_area(vector<RectangleUnionRectangle<T>> rectangles)
```

`rectangles` は `RectangleUnionRectangle<T>{x1, y1, x2, y2}` の列である。各長方形は半開領域 $[x_1, x_2) \times [y_1, y_2)$ として扱う。

y 方向の被覆回数を管理するセグメント木を使った平面走査で求める。

## 時間計算量・空間計算量

$N$ を入力長方形数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `rectangle_union_area(rectangles)` | $O(N\log N)$ | $O(N)$ |
| `RectangleUnionRectangle` の各座標メンバの参照 | $O(1)$ | $O(1)$ |

前処理として座標圧縮と $2N$ 個以下のイベントの整列を行い、各イベントを $O(\log N)$ で処理する。

## 注意点

- `x1 > x2` や `y1 > y2` の長方形は端点を入れ替えて扱う。幅または高さが $0$ の長方形は無視する。
- 空入力（または有効な長方形がない場合）は `T(0)` を返す。
- 面積は `T` 上で計算するため、`T` で表現できる範囲に収まらなければならない。
