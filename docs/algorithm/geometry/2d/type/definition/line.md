---
title: 2D Line (二次元直線)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/line.hpp
---

2 点を通る無限直線を表す集成体型。

## API

```cpp
struct Line {
    Point a;
    Point b;
};
```

- `Line{a, b}`: 通過する 2 点 `a`, `b` から構築する。

## 計算量

構築とフィールドの参照は時間 $O(1)$、追加空間 $O(1)$。

## 注意点

直線を使う API では通常 `a != b` が必要である。閉線分には `Segment` を使う。
