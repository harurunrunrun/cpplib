---
title: Minimum Enclosing Rectangle Result (最小外接長方形の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/minimum_enclosing_rectangle_result.hpp
---

凸集合を含む面積最小の長方形を保持する結果型。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/type/definition/minimum_enclosing_rectangle_result.hpp"
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

- `corners`: 非退化な長方形では反時計回りに並ぶ 4 頂点。
- `width`, `height`: 隣り合う 2 辺の長さ。
- `area`: `width * height`。
- `valid`: 空でない入力に対して `true`。

デフォルト構築では全座標と数値が $0$、`valid == false` となる。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| デフォルト構築、全フィールドの参照 | $O(1)$ | $O(1)$ |

## 注意点

この型は元の凸多角形を保持しない。浮動小数点演算のため、`area` と
`width * height` の間には丸め誤差が生じ得る。点または線分に対しては高さと面積が $0$ で、
`corners` に重複する点を含む。

## 使用例

```cpp
MinimumEnclosingRectangleResult result;
assert(!result.valid);
assert(result.area == 0.0L);
```
