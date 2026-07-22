---
title: 3D Fixed Matrix (三次元固定行列)
documentation_of: ../src/algorithm/geometry/3d/core/matrix3.hpp
---

`Matrix3` は `long double` の $3\times3$ 行列である。要素は固定長の
`std::array<std::array<long double, 3>, 3>` に行優先で保持する。

## API

- `Matrix3()`: 全要素が0の行列を構築する。
- `Matrix3(storage)`: `storage[row][column]` から行列を構築する。
- `matrix[row][column]`: 指定要素を参照する。
- `Matrix3::identity()`: 単位行列を返す。
- `+matrix`, `-matrix`: 単項符号を適用する。
- `left + right`, `left - right`, `operator+=`, `operator-=`: 要素ごとの和・差を求める。
- `left * right`, `operator*=`: 行列積を求める。
- `matrix * point`: `Point3` を列ベクトルとみなした積を返す。
- `matrix.transposed()`: 転置行列を返す。
- `matrix.determinant()`: 行列式を返す。
- `matrix.inverse(relative_epsilon)`: 逆行列を返す。特異または数値的に特異なら `nullopt`。
- `matrix.solve(rhs, relative_epsilon)`: $Ax=rhs$ の一意解を返す。数値的に特異なら `nullopt`。

## API別の時間計算量・空間計算量

- `Matrix3()`、`Matrix3(storage)`、`matrix[row]`、`identity()`、単項符号、
  和・差、`transposed()`、`determinant()`、`matrix * point`、`inverse()`、
  `solve()`: 時間・追加領域 $O(1)$。
- `left * right` と `operator*=`: $3^3$ 回の固定長演算なので時間・追加領域
  $O(1)$。

すべて固定サイズの演算であり、時間・追加領域は $O(1)$。`inverse` と `solve` の
返却値も固定サイズである。

## 注意点

- `inverse` と `solve` は
  全要素を `max_abs_entry` で割った正規化行列について
  `abs(det) <= relative_epsilon` を数値的特異と判定する。
  正規化行列を反転してから結果を `max_abs_entry` で割るため、元行列の
  行列式や余因子がoverflowする一様scaleでも、逆行列が表現可能なら成功する。
- `relative_epsilon` は有限な非負値を指定する。不正値、非有限要素、非有限な
  正規化行列式、または表現不能な逆行列では `nullopt` を返す。
- 添字は `[0, 3)` でなければならない。`operator[]` は範囲検査を行わない。
- `determinant()`、通常の行列積、`matrix * point` は入力を正規化しないため、
  有限な入力でも結果が表現範囲を超えることがある。
