---
title: 2D Common Tangents (二次元円の共通接線)
documentation_of: ../src/algorithm/geometry/2d/common_tangents.hpp
---

## API

- `common_tangents(first, second)`: 2円に共通する異なる接線を0～4本返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

一致する円は接線が無限にあるため `std::domain_error`、負半径には `std::invalid_argument` を送出する。
