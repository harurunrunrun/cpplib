---
title: Quaternion Dot Product (クォータニオンの内積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/quaternion_quaternion/quaternion3_dot.hpp
---

2 つのクォータニオンを 4 次元ベクトルとみなした内積を計算する。

## API

- `quaternion3_dot(first, second)`: $w_1 w_2 + x_1 x_2 + y_1 y_2 + z_1 z_2$ を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 両入力は有限値でなければならない。非有限な入力には `std::invalid_argument` を、結果のオーバーフローには `std::overflow_error` を送出する。
