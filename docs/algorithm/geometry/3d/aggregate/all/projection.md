---
title: 3D Projection (三次元射影)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/projection.hpp
---

## API

いずれも戻り値は `Point3` である。

- `projection(line, point)`: 点を無限直線に正射影する。
- `projection(plane, point)`: 点を平面に正射影する。

## API 別の時間計算量・空間計算量

- 各オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 退化した直線、零ベクトルの法線、非有限な入力には `std::invalid_argument` を送出する。
- 基準点からの差分を局所スケールに正規化し、直線では専用の $O(1)$ の式、平面では単位法線を用いて、FMA で射影点を復元する。射影の係数が厳密に 0 なら、入力上の点をそのまま返す。
- 結果、または局所的に正規化した差分を `long double` で表現できない場合は `std::overflow_error` を送出する。
