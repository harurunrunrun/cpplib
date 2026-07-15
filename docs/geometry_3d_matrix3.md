---
title: 3D Fixed Matrix (三次元固定行列)
documentation_of: ../src/algorithm/geometry/3d/matrix3.hpp
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

すべて固定サイズの演算であり、時間・追加領域は $O(1)$。`inverse` と `solve` の
返却値も固定サイズである。

## 注意点

- `inverse` と `solve` は
  `abs(det) <= relative_epsilon * max_abs_entry^3` を数値的特異と判定する。
- `relative_epsilon` は有限な非負値を指定する。不正値、非有限要素、非有限な
  行列式では `nullopt` を返す。
- 添字は `[0, 3)` でなければならない。`operator[]` は範囲検査を行わない。
- 入力と中間値が有限な `long double` の範囲に収まる必要がある。
