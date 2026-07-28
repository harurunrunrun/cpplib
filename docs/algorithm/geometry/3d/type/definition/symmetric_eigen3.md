---
title: Symmetric 3x3 Eigensystem Result (三次対称行列の固有系結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/symmetric_eigen3.hpp
---

三次実対称行列の固有値と対応する正規直交固有ベクトルを保持する型。

## API

- `eigenvalues`: 固有値を格納する長さ3の配列。
- `eigenvectors`: 第 $i$ 列に `eigenvalues[i]` と対応する固有ベクトルを持つ行列。
- `eigenvector(i)`: 第 $i$ 固有ベクトルを `Point3` として返す。

## API別の時間計算量・空間計算量

- 各フィールドへの参照と `eigenvector(i)`: 時間・追加領域ともに $O(1)$。
- 結果型が保持する領域: $O(1)$。

## 注意点

`eigenvector(i)` は $i\geq 3$ で `std::out_of_range` を送出する。この型自身は固有値の順序、固有ベクトルの正規直交性、対応関係を検査しない。
