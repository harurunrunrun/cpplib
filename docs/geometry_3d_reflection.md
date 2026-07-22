---
title: 3D Reflection (三次元鏡映)
documentation_of: ../src/algorithm/geometry/3d/point/reflection.hpp
---

## API

- `reflection(line, point)`: 点を直線の周りに180度回転した点を返す。
- `reflection(plane, point)`: 点を平面に関して鏡映した点を返す。

## API別の時間計算量・空間計算量

- 各overload: 時間・追加領域 $O(1)$。

## 注意点

退化直線、零法線、非有限入力には `std::invalid_argument` を送出する。射影点との差を
局所scaleへ正規化し、FMAで鏡映点を一度に復元する。結果を `long double` で
表現できない場合は `std::overflow_error` を送出する。
