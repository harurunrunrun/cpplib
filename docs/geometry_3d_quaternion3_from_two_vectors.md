---
title: Quaternion between Two Vectors (二ベクトル間の最短回転)
documentation_of: ../src/algorithm/geometry/3d/quaternion3_from_two_vectors.hpp
---

## API

- `quaternion3_from_two_vectors(from, to)`: `from`の向きを`to`の向きへ移す最短回転の単位`Quaternion3`を返す。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

両ベクトルは有限な非零ベクトルでなければならない。同方向なら恒等回転、反対方向なら一意でない回転軸を座標順に決定して$\pi$回転を返す。長さは無視する。
