---
title: Distance between 3D Ray and Segment (三次元半直線と線分の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/ray_segment_distance.hpp
---

## API

- `distance(first, second)`: `Ray3` と `Segment3` の集合としての距離を返す。異なる型の組なので、引数を逆順にしたオーバーロードも提供する。
- この互換用の集約ヘッダは、`ray_segment` 順と `segment_ray` 順の両方の個別ヘッダを読み込む。

## API別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加領域ともに $O(1)$。

## 注意点

交わる場合（重なる場合を含む）は 0 を返す。線分は退化（両端点が一致）していてもよい。退化した半直線・非有限な入力には `std::invalid_argument` を送出する。
最近点対を局所座標で求めてから距離を復元し、距離を `long double` で表現できない場合は `std::overflow_error` を送出する。
