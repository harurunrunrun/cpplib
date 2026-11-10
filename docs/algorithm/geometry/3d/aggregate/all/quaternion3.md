---
title: Quaternion3 (3次元クォータニオン)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/quaternion3.hpp
---

`Quaternion3` 型と内積関数をまとめて include する、互換のための集約ヘッダである。型だけが必要なら `quaternion3_type.hpp`、内積だけが必要なら `quaternion3_dot.hpp` を直接 include すればよい。

## API

- `Quaternion3`: 成分 `w, x, y, z`（`long double`、デフォルトは 0）を持つ四元数。`identity()`、`is_finite()`、単項の `+` / `-`、四元数同士の加算・減算・乗算、スカラー倍とスカラーでの除算（それぞれ複合代入も）、共役 `conjugate()`、ノルム `norm()`、正規化 `normalized()`、逆元 `inverse()` を提供する。四元数同士の除算演算子はない（`inverse()` との積で代用する）。
- `quaternion3_dot(first, second)`: 4 成分の内積を返す。

## API 別の時間計算量・空間計算量

すべて時間・追加空間ともに $O(1)$。

## 注意点

- 入力は有限値でなければならない。非有限なスカラーや、0 または非有限な除数には `std::invalid_argument` を送出する。
- 零四元数の正規化・逆元には `std::invalid_argument` を送出する。
- ノルムや演算結果がオーバーフローする場合は `std::overflow_error` を送出する。
