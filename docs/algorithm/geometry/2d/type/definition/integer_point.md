---
title: 2D Integer Point (二次元整数点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/integer_point.hpp
---

整数座標の二次元の点（ベクトル）を表す集成体型。

## API

```cpp
template<std::integral Coordinate>
struct IntegerPoint {
    Coordinate x{};
    Coordinate y{};
    bool operator==(const IntegerPoint&) const = default;
    bool operator<(const IntegerPoint& other) const;
};
```

- `IntegerPoint<Coordinate>{x, y}`: 座標を指定して構築する。デフォルト構築では両座標が $0$ になる。
- `operator==`: 座標が完全に一致するかを返す。
- `operator<`: $(x, y)$ の辞書順で比較する。

## 計算量

構築・比較は時間 $O(1)$、追加空間 $O(1)$。

## 注意点

浮動小数点数用の `Point` との暗黙の変換は行わない。
