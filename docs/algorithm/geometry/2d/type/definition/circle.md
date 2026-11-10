---
title: 2D Circle (二次元円)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/circle.hpp
---

中心と半径で円を表す集成体型。

## API

```cpp
struct Circle {
    Point center;
    long double radius;
};
```

- `Circle{center, radius}`: 中心と半径から構築する。

## 時間計算量・空間計算量

構築とフィールドの参照は時間 $O(1)$、追加空間 $O(1)$。

## 注意点

型自体は半径の有限性・非負性を検査しない。必要に応じて `validate_circle` で検査する。
