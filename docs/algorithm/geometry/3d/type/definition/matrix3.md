---
title: 3D Fixed Matrix (三次元固定行列)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/matrix3.hpp
---

`Matrix3` は `long double` を要素とする $3 \times 3$ 行列である。要素は固定長の `std::array<std::array<long double, 3>, 3>` に行優先で保持する。

## API

- `Matrix3()`: 全要素が $0$ の行列を構築する。
- `Matrix3(storage)`: `storage[row][column]` から行列を構築する（`explicit`）。
- `matrix[row][column]`: 指定した要素を参照する。
- `Matrix3::identity()`: 単位行列を返す。
- `+matrix`, `-matrix`: 単項の符号演算を適用する。
- `left + right`, `left - right`, `operator+=`, `operator-=`: 要素ごとの和・差を求める。
- `left * right`, `operator*=`: 行列積を求める。
- `matrix * point`: `Point3` を列ベクトルとみなした積を返す。
- `matrix.transposed()`: 転置行列を返す。
- `matrix.determinant()`: 行列式を返す。
- `matrix.inverse(relative_epsilon = GEOMETRY3D_EPS)`: 逆行列を `std::optional<Matrix3>` で返す。特異または数値的に特異なら `std::nullopt` を返す。
- `matrix.solve(rhs, relative_epsilon = GEOMETRY3D_EPS)`: $Ax = \mathrm{rhs}$ の一意解を `std::optional<Point3>` で返す。数値的に特異なら `std::nullopt` を返す。

## API別の時間計算量・空間計算量

すべて固定サイズの演算（行列積でも $3^3$ 回の積和）であり、どの API も時間・追加領域は $O(1)$。`inverse` と `solve` の返り値も固定サイズである。

## 注意点

- `inverse` と `solve` は、全要素を要素の絶対値の最大値で割った正規化行列について、行列式の絶対値が `relative_epsilon` 以下なら数値的に特異と判定する。正規化行列の逆行列を求めてから結果を同じ最大値で割るため、元の行列の行列式や余因子がオーバーフローするような一様なスケールでも、逆行列が表現可能なら成功する。
- `relative_epsilon` には有限な非負値を指定する。不正な値、非有限な要素、零行列、正規化行列の行列式が非有限の場合、逆行列が表現できない場合は `std::nullopt` を返す。
- 添字は $[0, 3)$ でなければならない。`operator[]` は範囲検査を行わない。
- `determinant()`、通常の行列積、`matrix * point`、および `solve` の最後の積は入力を正規化しないため、有限な入力でも結果が表現範囲を超えることがある。
