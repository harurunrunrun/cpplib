---
title: Line-Line Distance (直線間距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/line_line/line_line_distance.hpp
---

## API

- `distance(first, second)`: 2 直線間の最短距離を返す。平行な場合は一方の点ともう一方の直線との距離になる。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域ともに $O(1)$。

## 注意点

- 両直線は退化しておらず、座標は有限値でなければならない。違反時は `std::invalid_argument` を送出する。
- 距離を `long double` で表現できない場合は `std::overflow_error` を送出する。
