---
title: 3D Triangle Area (三次元三角形面積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/triangle/area.hpp
---

## API

- `area(triangle)`: 三角形の面積（非負） $\lVert (b - a) \times (c - a) \rVert / 2$ を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 退化した三角形では $0$ を返す（浮動小数点誤差により厳密に $0$ になるとは限らない）。
- 座標は有限値でなければならない。外積が非有限値になった場合（座標が巨大な場合）は `std::invalid_argument` を送出する。
