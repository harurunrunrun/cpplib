---
title: Symmetric 3x3 Eigensystem (三次対称行列の固有系)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/matrix/symmetric_eigen_3x3.hpp
---

実対称な `Matrix3` の固有値と正規直交固有ベクトルを Jacobi 法（Jacobi 回転の反復）で求める。

## API

- `symmetric_eigen_3x3(matrix, relative_epsilon = GEOMETRY3D_EPS)`: 固有値を降順に並べた `SymmetricEigen3` を返す。`eigenvalues[i]` が $i$ 番目の固有値、`eigenvectors` の第 $i$ 列（`eigenvector(i)` で取得できる）が対応する単位固有ベクトルである。

## API別の時間計算量・空間計算量

行列のサイズと最大反復回数（80 回）が固定なので、時間・追加領域ともに $O(1)$。

## 注意点

- 入力は最大絶対値成分で正規化してから扱う。非対角成分の差が最大絶対値成分の `relative_epsilon` 倍以内であれば対称とみなし、対称な成分の平均を使う。
- 非対称な行列、非有限な成分、負または非有限な `relative_epsilon` には `std::invalid_argument` を送出する。
- 零行列に対しては、固有値がすべて 0 で固有ベクトルが単位行列の列である結果を返す。
- 固有値が `long double` で表現できない場合は `std::overflow_error` を送出する。
- 重複する固有値に属する個々の固有ベクトルは一意でない。各固有ベクトルは、絶対値最大の成分が正になるよう符号だけを正規化する。
