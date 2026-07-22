---
title: Quaternion to Matrix3 (クォータニオンから回転行列への変換)
documentation_of: ../src/algorithm/geometry/3d/shape/quaternion3_to_matrix3.hpp
---

## API

- `quaternion3_to_matrix3(quaternion)`: クォータニオンと同じ右手系回転を表す$3\times3$行列を返す。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

入力は有限な非零クォータニオンでなければならず、内部で正規化される。返す行列は理論上直交行列かつ行列式1で、`matrix * point`は`quaternion3_rotate(quaternion, point)`と一致する。
