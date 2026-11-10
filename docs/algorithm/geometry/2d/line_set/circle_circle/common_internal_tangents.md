---
title: 2D Common Internal Tangents (二次元円の共通内接線)
documentation_of: ../../../../../../src/algorithm/geometry/2d/line_set/circle_circle/common_internal_tangents.hpp
---

## API

- `std::vector<Line> common_internal_tangents(first, second)`: 2 円の共通内接線を返す。

## 引数

`first`, `second` は非負の半径を持つ円である。

## 戻り値

相異なる共通内接線を 0〜2 本返す。中心が（許容誤差の範囲で）一致し半径が異なる場合は空を返す。

## API別の時間計算量・空間計算量

時間・出力・追加領域とも $O(1)$。

## 注意点

- 2 円が一致する場合は共通接線が無限に存在するため `std::domain_error` を送出する。
- 非有限な座標・半径や負の半径には `std::invalid_argument` を送出する。
- 判定は `common_tangents` と同じく、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で行う。
