---
title: 3D Tetrahedron Incenter (三次元四面体の内心)
documentation_of: ../src/algorithm/geometry/3d/point/tetrahedron_incenter.hpp
---

## API

- `tetrahedron_incenter(tetrahedron)`: 4面から等距離にある内心を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化四面体・非有限入力には `std::invalid_argument` を送出する。退化判定には正確な
`adaptive_orient3d` の符号を用いる。anchorからの辺差分を共通scaleへ正規化し、
局所面積を重みにしてFMAで中心を復元する。局所形状または結果を表現できない場合は
`std::overflow_error` を送出する。
