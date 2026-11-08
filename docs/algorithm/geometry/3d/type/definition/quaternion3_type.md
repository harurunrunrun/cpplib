---
title: Quaternion3 Type (三次元クォータニオン型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/quaternion3_type.hpp
---

実部 1 個と虚部 3 個を `long double` で持つクォータニオン型を提供する。

## API

- `Quaternion3{w, x, y, z}`: 4 成分を保持する。各成分のデフォルト値は 0。
- `Quaternion3::identity()`: 乗法単位元 $(1, 0, 0, 0)$ を返す。
- `is_finite()`: 全成分が有限値かを判定する。
- 単項 `+`, `-`、二項 `+`, `-` とその複合代入: 成分ごとの演算。
- `operator*(Quaternion3)` とその複合代入: ハミルトン積。
- `operator*(scalar)`（左右どちらからでも可）, `operator/(scalar)` とその複合代入: スカラー倍・スカラーでの除算。
- `conjugate()`: 共役 $(w, -x, -y, -z)$ を返す。
- `norm()`: ユークリッドノルムを返す。オーバーフローを避けるため、最大絶対値の成分で割ってから計算する。
- `normalized()`: 同じ向きの単位クォータニオンを返す。
- `inverse()`: 乗法逆元を返す。

## API別の時間計算量・空間計算量

すべて時間・追加領域ともに $O(1)$。

## 注意点

- 演算の入力に有限でない成分（NaN・無限大）が含まれると `std::invalid_argument` を送出する。
- 零クォータニオンの正規化・逆元、有限でないスカラーによる乗算・除算、0 による除算も `std::invalid_argument` を送出する。
- 結果が表現範囲を超えて有限でなくなる場合は `std::overflow_error` を送出する。
