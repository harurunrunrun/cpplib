---
title: Quaternion Dot Product (クォータニオンの内積)
documentation_of: ../../../../src/algorithm/geometry/3d/quaternion3_dot.hpp
---

2個のクォータニオンを4次元ベクトルとみなした内積を計算します。

## API

- `quaternion3_dot(first, second)`: $w_1w_2+x_1x_2+y_1y_2+z_1z_2$ を返します。

## API別の時間計算量・空間計算量

時間・追加空間ともに $O(1)$ です。

## 注意点

両入力は有限値でなければなりません。非有限入力は `std::invalid_argument`、結果のoverflowは `std::overflow_error` になります。