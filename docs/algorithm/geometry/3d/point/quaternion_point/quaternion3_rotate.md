---
title: Quaternion Vector Rotation (クォータニオンによるベクトル回転)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/quaternion_point/quaternion3_rotate.hpp
---

## API

- `quaternion3_rotate(quaternion, point)`: `quaternion` が表す回転を、原点を中心として `point` に適用した点を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- クォータニオンは内部で正規化されるため零であってはならない。点とクォータニオンの全成分は有限でなければならない。不正な入力には `std::invalid_argument` を送出する。
- 結果がオーバーフローした場合は `std::overflow_error` を送出する。
- 回転なので点の長さは保たれる。
