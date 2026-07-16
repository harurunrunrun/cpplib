---
title: 3D Projection (三次元射影)
documentation_of: ../src/algorithm/geometry/3d/projection.hpp
---

## API

- `projection(line, point)`: 点を無限直線へ正射影する。
- `projection(plane, point)`: 点を平面へ正射影する。

## API別の時間計算量・空間計算量

- 各overload: 時間・追加領域 $O(1)$。

## 注意点

退化直線、零法線、非有限入力には `std::invalid_argument` を送出する。anchorからの
差分を局所scaleへ正規化し、直線では専用の $O(1)$ 式、平面では単位法線を用いて
FMAで射影点を復元する。射影係数が厳密に0なら入力上の点をそのまま返す。結果または
局所正規化差分を `long double` で表現できない場合は `std::overflow_error` を送出する。
