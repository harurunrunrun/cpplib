---
title: Symmetric 3x3 Eigensystem (三次対称行列の固有系)
documentation_of: ../src/algorithm/geometry/3d/symmetric_eigen_3x3.hpp
---

実対称な `Matrix3` の固有値と正規直交固有ベクトルをJacobi回転で求める。

## API

- `symmetric_eigen_3x3(matrix, relative_epsilon)`: `SymmetricEigen3` を返す。
  `eigenvalues` は降順、`eigenvectors` の第 $i$ 列が対応する固有ベクトル。
- `result.eigenvector(i)`: 第 $i$ 固有ベクトルを `Point3` で返す。

## 時間計算量

行列sizeと最大反復回数が固定なので時間・追加領域ともに $O(1)$。

## 注意点

- `relative_epsilon` の範囲で対称な入力は、対称成分の平均を使う。
- 非対称、非有限、不正な許容誤差には `std::invalid_argument` を送出する。
- 固有値が `long double` に表現不能なら `std::overflow_error` を送出する。
- 重複固有値に属する個々の固有ベクトルは一意でない。最大絶対成分が正になるよう
  符号のみ正規化する。
