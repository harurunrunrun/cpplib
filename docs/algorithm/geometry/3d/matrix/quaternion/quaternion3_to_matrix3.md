---
title: Quaternion to Matrix3 (クォータニオンから回転行列への変換)
documentation_of: ../../../../../../src/algorithm/geometry/3d/matrix/quaternion/quaternion3_to_matrix3.hpp
---

## API

- `quaternion3_to_matrix3(quaternion)`: クォータニオンと同じ右手系の回転を表す $3\times 3$ 行列を返す。

## API別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

入力は有限で非零のクォータニオンでなければならず、違反時は `std::invalid_argument` を送出する。内部で正規化してから変換する。
返す行列は理論上は行列式 1 の直交行列であり、`matrix * point` は `quaternion3_rotate(quaternion, point)` と一致する。
