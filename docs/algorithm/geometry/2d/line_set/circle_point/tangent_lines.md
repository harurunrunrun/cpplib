---
title: 2D Tangent Lines from Point (二次元点から円への接線)
documentation_of: ../../../../../../src/algorithm/geometry/2d/line_set/circle_point/tangent_lines.hpp
---

## API

- `std::vector<Line> tangent_lines(circle, point)`: `point` を通る円の相異なる接線を 0〜2 本返す。

## 戻り値

- 点が円の内部にあれば空。
- 点が（許容誤差の範囲で）円周上にあれば、その点で円に接する 1 本。
- 点が外部にあれば 2 本。各直線は `point` と接点を通り、接点の辞書順に並ぶ。
- 半径 0 の円では、`point` と中心を通る 1 本を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

- 円の中心座標と半径は有限値であること。非有限値または負の半径には `std::invalid_argument` を送出する。
- 半径 0 の円で `point` が中心と完全に一致する場合は、接線が一意に定まらないため `std::domain_error` を送出する。
- 円周上かどうかの判定は、距離の 2 乗と半径の 2 乗の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で行う。中間値は有限な `long double` に収まる必要がある。
