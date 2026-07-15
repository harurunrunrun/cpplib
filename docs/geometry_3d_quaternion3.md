---
title: Quaternion3 (三次元クォータニオン)
documentation_of: ../src/algorithm/geometry/3d/quaternion3.hpp
---

## API

- `Quaternion3{w, x, y, z}`: 実部`w`と虚部`(x,y,z)`を持つクォータニオンを構築する。
- `Quaternion3::identity()`: 乗法単位元$(1,0,0,0)$を返す。
- `is_finite()`: 全成分が有限なら`true`を返す。
- 単項`+`, `-`および二項`+`, `-`: 符号反転・成分ごとの加減算を行う。複合代入にも対応する。
- `operator*(Quaternion3)`: Hamilton積を返す。複合代入にも対応する。
- `operator*(scalar)`, `operator/(scalar)`: スカラー倍・除算を返す。複合代入にも対応する。
- `conjugate()`: 共役$(w,-x,-y,-z)$を返す。
- `norm()`: Euclidノルムを返す。
- `normalized()`: 単位クォータニオンへ正規化して返す。
- `inverse()`: 乗法逆元を返す。
- `quaternion3_dot(a,b)`: 4成分ベクトルとしての内積を返す。

## API別の時間計算量・空間計算量

すべてのAPIは時間$O(1)$、追加領域$O(1)$。

## 注意点

成分とスカラーは有限値でなければならない。零クォータニオンの正規化・逆元、および0除算は`std::invalid_argument`、表現可能範囲を超えた演算は`std::overflow_error`を送出する。回転を表すとき$q$と$-q$は同じ回転である。
