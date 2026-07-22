---
title: Great-Circle Distance (大円距離)
documentation_of: ../src/algorithm/geometry/3d/scalar/great_circle_distance.hpp
---

## API

- `great_circle_distance(first, second, radius=1)`: 半径`radius`の球面上で2方向を結ぶ短い大円弧の長さを返す。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

方向は有限な非零ベクトル、半径は有限な非負値でなければならない。負半径などには`std::invalid_argument`、距離が表現可能範囲を超える場合は`std::overflow_error`を送出する。
