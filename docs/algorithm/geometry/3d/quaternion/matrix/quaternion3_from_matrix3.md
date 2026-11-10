---
title: Matrix3 to Quaternion (回転行列からクォータニオンへの変換)
documentation_of: ../../../../../../src/algorithm/geometry/3d/quaternion/matrix/quaternion3_from_matrix3.hpp
---

## API

- `quaternion3_from_matrix3(matrix, tolerance = 1e-9L)`: 回転行列 `matrix` を単位 `Quaternion3` に変換して返す。成分を $(w, x, y, z)$ の順に見て、最初の非零成分が正になるよう符号を正規化する。

## API別の時間計算量・空間計算量

時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- `matrix` は有限な直交行列で、かつ行列式が $1$ でなければならない。
- `tolerance` は $[0, 1)$ の有限値で、直交性（$M M^{\top}$ の各成分）と行列式の判定に絶対・相対許容誤差の両方として使う（`relative_equal` を用いる）。
- `tolerance` が範囲外の場合や、非有限・非直交・反射（行列式が $-1$）・特異な行列には `std::invalid_argument` を送出する。
- $q$ と $-q$ は同じ回転を表すので、上記のように戻り値の符号を一意に定める。
