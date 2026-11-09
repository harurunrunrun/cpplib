---
title: 2D Common Tangents (二次元円の共通接線)
documentation_of: ../../../../../../src/algorithm/geometry/2d/line_set/circle_circle/common_tangents.hpp
---

## API

- `std::vector<Line> common_tangents(first, second)`: 2 円に共通する相異なる接線を 0〜4 本返す。共通内接線、共通外接線の順に並ぶ。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

- 2 円が一致する場合は接線が無限に存在するため `std::domain_error` を送出する。中心が一致し半径が異なる場合は空を返す。
- 円の中心座標と半径は有限値であること。非有限値または負の半径には `std::invalid_argument` を送出する。
- 接触・包含の判定（距離の 2 乗など）は、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で行う。中間値は有限な `long double` に収まる必要がある。
- 接点が 2 円で一致する場合（2 円が接する場合）は、接点を通り中心線に垂直な直線を返す。同じ直線は重複して返さない。
