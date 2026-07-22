---
title: Quaternion Vector Rotation (クォータニオンによるベクトル回転)
documentation_of: ../src/algorithm/geometry/3d/point/quaternion3_rotate.hpp
---

## API

- `quaternion3_rotate(quaternion, point)`: `quaternion`が表す回転を原点基準で`point`に適用する。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

クォータニオンは内部で正規化されるため零であってはならず、点と全成分は有限でなければならない。不正入力には`std::invalid_argument`、結果のoverflowには`std::overflow_error`を送出する。点の長さを保存する。
