---
title: Quaternion from Axis-Angle (軸角からクォータニオンへの変換)
documentation_of: ../src/algorithm/geometry/3d/shape/quaternion3_from_axis_angle.hpp
---

## API

- `quaternion3_from_axis_angle(axis, angle)`: 右手系で`axis`の周りを`angle`ラジアン回転する単位`Quaternion3`を返す。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

`axis`は有限な非零ベクトル、`angle`は有限値でなければならない。不正入力には`std::invalid_argument`を送出する。軸の長さは結果に影響しない。
