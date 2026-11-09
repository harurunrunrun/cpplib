---
title: 3D Reflection (三次元鏡映)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/reflection.hpp
---

## API

- `reflection(line, point)`: 点を直線の周りに 180 度回転した点（直線に関する対称点）を返す。
- `reflection(plane, point)`: 点を平面に関して鏡映した点を返す。

この互換用の集約ヘッダは、上の 2 つの個別ヘッダを読み込む。

## API別の時間計算量・空間計算量

- 各オーバーロード: 時間・追加領域ともに $O(1)$。

## 注意点

退化した直線、法線が零ベクトルの平面、非有限な入力には `std::invalid_argument` を送出する。
射影点との差を局所スケールで正規化し、FMA で鏡映点を一度に復元する。結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
