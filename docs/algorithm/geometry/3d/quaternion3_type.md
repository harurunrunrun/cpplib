---
title: Quaternion3 Type (3次元クォータニオン型)
documentation_of: ../../../../src/algorithm/geometry/3d/quaternion3_type.hpp
---

実部と3個の虚部を持つクォータニオン型を提供します。

## API

- `Quaternion3{w, x, y, z}`: 4成分を保持します。
- `Quaternion3::identity()`: 乗法単位元 $(1,0,0,0)$ を返します。
- `is_finite()`: 全成分が有限値か判定します。
- 単項 `+`, `-`、二項 `+`, `-` と各複合代入: 成分ごとの演算です。
- `operator*(Quaternion3)`: Hamilton積を返します。
- `operator*(scalar)`, `operator/(scalar)` と各複合代入: スカラー倍・除算です。
- `conjugate()`: 共役 $(w,-x,-y,-z)$ を返します。
- `norm()`: Euclidノルムを返します。
- `normalized()`: 単位クォータニオンを返します。
- `inverse()`: 乗法逆元を返します。

## API別の時間計算量・空間計算量

すべて時間・追加空間ともに $O(1)$ です。

## 注意点

演算入力は有限値でなければなりません。零クォータニオンの正規化・逆元と零除算は `std::invalid_argument`、表現範囲を超える結果は `std::overflow_error` になります。