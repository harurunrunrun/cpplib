---
title: 2D Ray (二次元半直線)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/ray.hpp
---

始点と進行方向上の 1 点で半直線を表す集成体型。

## API

```cpp
struct Ray {
    Point origin;
    Point through;
};
```

- `Ray{origin, through}`: `origin` は始点、`through` は進行方向上の点である。

## 時間計算量・空間計算量

構築: 時間 $O(1)$、空間 $O(1)$。

## 注意点

半直線を使う演算では `origin != through` が必要である。
