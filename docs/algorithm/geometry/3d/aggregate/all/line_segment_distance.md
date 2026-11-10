---
title: Distance between 3D Line and Segment (三次元直線と線分の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_segment_distance.hpp
---

## API

- `distance(first, second)`: `Line3` と `Segment3` の集合としての距離を `long double` で返す。引数の型の順序を入れ替えたオーバーロードも提供する。
- この互換用集約ヘッダは `(Line3, Segment3)` 版と `(Segment3, Line3)` 版の両方の個別ヘッダを提供する。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 交差する場合、または重なる場合は 0 を返す。
- 線分は退化していてもよい。
- 退化した直線、非有限な入力には `std::invalid_argument` を送出する。
- 最近点対を局所座標で求める。距離を表現できない場合は `std::overflow_error` を送出する。
