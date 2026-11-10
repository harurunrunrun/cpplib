---
title: Solve 3x3 Linear System (三元連立一次方程式)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/matrix_point/solve_linear_system_3x3.hpp
---

`Matrix3` と `Point3` で与えた三元連立一次方程式 $Ax = b$ を解く。

## API

- `solve_linear_system_3x3(matrix, rhs, relative_epsilon = GEOMETRY3D_EPS)`: 一意解を `Point3` で返す。数値的に特異なら `std::nullopt` を返す。

## API別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 係数行列と右辺をそれぞれの最大絶対値で独立にスケーリングし、部分ピボット選択付きの Gauss の消去法で解く。
- スケーリング後のピボットの絶対値が `relative_epsilon` 以下になった場合（係数行列が零行列の場合を含む）は特異とみなして `std::nullopt` を返す。
- 非有限な値や、負または非有限な `relative_epsilon` には `std::invalid_argument` を送出する。解が `long double` で表現できない場合は `std::overflow_error` を送出する。
