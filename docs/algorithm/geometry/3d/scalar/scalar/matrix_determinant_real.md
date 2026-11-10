---
title: Real Matrix Determinant (実数行列の行列式)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/scalar/matrix_determinant_real.hpp
---

`long double` の一般の正方行列の行列式を、部分ピボット選択付きの消去法で求める。

## API

- `matrix_determinant_real(matrix, relative_epsilon = GEOMETRY3D_EPS)`: 行列式を返す。$0 \times 0$ 行列の値は $1$、数値的に特異な行列の値は $0$ とする。

## 時間計算量・空間計算量

- $N \times N$ 行列に対し、時間 $O(N^3)$、追加領域 $O(N^2)$。

## 注意点

- 行列全体を最大絶対値の要素で正規化し、ピボットの絶対値が `relative_epsilon` 以下になったら特異とみなして $0$ を返す。
- 正方でない、行の長さが不一致、非有限な値を含む、許容誤差が不正（負または非有限）な入力には `std::invalid_argument` を送出する。
- ピボットの積は仮数と 2 進指数に分けて蓄積し、中間積の不要なオーバーフローを避ける。消去中や最終値が表現できない場合は `std::overflow_error` を送出する。最終値がアンダーフローする場合は符号付きの $0$ を返す。
