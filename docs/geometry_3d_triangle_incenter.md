---
title: 3D Triangle Incenter (三次元三角形の内心)
documentation_of: ../src/algorithm/geometry/3d/point/triangle_incenter.hpp
---

## API

- `triangle_incenter(triangle)`: 三辺から等距離にある内心を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化三角形・非有限入力には `std::invalid_argument` を送出する。退化判定はexact
dyadic predicateで行い、辺差分を共通の局所scaleへ正規化して辺長と重みを求める。
中心はFMAでanchorへ復元する。局所形状または結果を `long double` で表現できない
場合は `std::overflow_error` を送出する。
