---
title: Least Squares by QR (QR分解による最小二乗法)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/scalar/least_squares.hpp
---

過決定または正方な実数線形系について、$\lVert Ax - b \rVert_2$ を最小化する一意な解を modified Gram–Schmidt 法による QR 分解で求める。各列を正規化し、直交化を 2 回行って丸め誤差を抑える。

## API

- `least_squares(matrix, right_hand_side, relative_epsilon = GEOMETRY3D_EPS)`: 列フルランクなら最小二乗解を `std::vector<long double>` で返す。列ランクが不足する場合や、行数が列数未満の場合は `std::nullopt` を返す。

## 計算量

- $R \times C$ 行列に対し、時間 $O(RC^2)$、追加領域 $O(RC + C^2)$。

## 注意点

- 列数が $0$ なら空の列を、右辺がすべて $0$ なら零ベクトルを返す。
- ランク判定は、正規化した列を直交化した残りの長さが `relative_epsilon` 以下かどうかで行う。零列を含む場合も `std::nullopt` を返す。
- ランク落ちの場合の解集合や Moore–Penrose 疑似逆行列は返さない。
- 行の長さや右辺のサイズの不一致、非有限な値、不正な許容誤差（負または非有限）には `std::invalid_argument` を送出する。
- 表現できない列ノルムや解には `std::overflow_error` を送出する。
