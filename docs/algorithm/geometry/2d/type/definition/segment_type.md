---
title: 2D Segment Type (二次元線分の型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/segment_type.hpp
---

二次元の閉線分を2端点で保持するaggregate型。

## API

```cpp
struct Segment {
    Point a;
    Point b;
};
```

`a`, `b` は向きを考えるAPIでは始点・終点、それ以外では交換可能な両端点として使う。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| aggregate構築、コピー、各field参照 | $O(1)$ | $O(1)$ |

## 注意点

`a == b` の退化線分も表現できる。型自体は座標の有限性や退化性を検査しない。
