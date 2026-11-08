---
title: 2D Common Tangent Points on First Circle (二次元第一円上の共通接点) [CGL_7_G]
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/circle_circle/common_tangent_points_on_first_circle.hpp
---

## API

- `std::vector<Point> common_tangent_points_on_first_circle(first, second)`: 2 円の各共通接線が `first` と接する点を、$(x, y)$ の辞書順に並べて返す。返す点の個数は $0$ ～ $4$ 個である。

## API別の時間計算量・空間計算量

- `common_tangent_points_on_first_circle`: 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

- 一致する円同士は共通接線が無限にあるため `std::domain_error` を送出する。負の半径または非有限値には `std::invalid_argument` を送出する。
- 半径 $0$ の円も円として扱う。
