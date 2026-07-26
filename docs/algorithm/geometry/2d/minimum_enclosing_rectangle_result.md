---
title: Minimum Enclosing Rectangle Result (最小外接長方形の結果)
documentation_of: ../../../../src/algorithm/geometry/2d/minimum_enclosing_rectangle_result.hpp
---

凸集合を含む面積最小の長方形を保持する結果型。

```cpp
#include "src/algorithm/geometry/2d/minimum_enclosing_rectangle_result.hpp"
```

## API

```cpp
struct MinimumEnclosingRectangleResult {
    std::array<Point, 4> corners;
    long double width;
    long double height;
    long double area;
    bool valid;
};
```

- `corners`: 非退化な長方形では反時計回りに並ぶ4頂点。
- `width`, `height`: 隣接する2辺の長さ。
- `area`: `width * height`。
- `valid`: 空でない入力に対して `true`。

既定構築では全座標と数値が0、`valid == false` となる。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 既定構築、全fieldの参照 | $O(1)$ | $O(1)$ |

## 注意点

この型は元の凸多角形を保持しない。浮動小数点演算により、`area` と
`width * height` の間には丸め誤差が生じ得る。点または線分では高さと面積が0で、
`corners` には重複点を含む。

## 使用例

```cpp
MinimumEnclosingRectangleResult result;
assert(!result.valid);
assert(result.area == 0.0L);
```
