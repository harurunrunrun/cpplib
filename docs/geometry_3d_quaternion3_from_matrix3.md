---
title: Matrix3 to Quaternion (回転行列からクォータニオンへの変換)
documentation_of: ../src/algorithm/geometry/3d/quaternion3_from_matrix3.hpp
---

## API

- `quaternion3_from_matrix3(matrix, tolerance=1e-9L)`: 適切な回転行列を単位`Quaternion3`へ変換し、符号を辞書順で正に正規化して返す。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

`matrix`は有限な直交行列かつ行列式が1でなければならない。`tolerance`は有限な$[0,1)$で、直交性と行列式の絶対・相対許容誤差に使う。反射・特異行列などには`std::invalid_argument`を送出する。$q$と$-q$が同じ回転なので返値の符号を一意化する。
