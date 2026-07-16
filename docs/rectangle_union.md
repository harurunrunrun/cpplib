---
title: Rectangle Union Area (長方形の和集合面積) [area_of_union_of_rectangles]
documentation_of: ../src/algorithm/other/rectangle_union.hpp
---

軸平行長方形の和集合の面積。

# 関数

```cpp
rectangle_union_area<T>(rectangles)
```

`rectangles` は `RectangleUnionRectangle<T>{x1, y1, x2, y2}` の列。

長方形は半開領域 $[x1, x2) \times [y1, y2)$ として扱う。

## 時間計算量

- $O(N \log N)$

## API別の時間計算量・空間計算量

$N$ を入力長方形数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `rectangle_union_area(rectangles)` | $O(N\log N)$ | $O(N)$ |
| `RectangleUnionRectangle` の各座標field参照 | $O(1)$ | $O(1)$ |

座標圧縮と $2N$ 個以下のevent整列を前処理として行い、各eventを $O(\log N)$ で処理する。

## 注意点

空入力、退化入力、範囲外のindexと容量超過の扱いは各APIの説明に従う。参照・iterator・callbackを受け渡すAPIでは、変更可否とobjectの寿命を守る。
