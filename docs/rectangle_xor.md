---
title: Rectangle XOR Area (長方形集合の排他的論理和面積)
documentation_of: ../src/algorithm/other/rectangle_xor.hpp
---

軸平行長方形の集合の xor の面積。

奇数個の長方形に含まれる領域の面積を返す。

# 関数

```cpp
rectangle_xor_area<T>(rectangles)
```

`rectangles` は `RectangleXorRectangle<T>{x1, y1, x2, y2}` の列。

長方形は半開領域 $[x1, x2) \times [y1, y2)$ として扱う。

## 時間計算量

- $O(N \log N)$

## API別計算量

$N$ を入力長方形数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `rectangle_xor_area(rectangles)` | $O(N\log N)$ | $O(N)$ |
| `RectangleXorRectangle` の各座標field参照 | $O(1)$ |

座標圧縮と $2N$ 個以下のevent整列を前処理として行い、各eventを $O(\log N)$ で処理する。
