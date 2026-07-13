---
title: 2D Circle Validation (二次元円の検査)
documentation_of: ../src/algorithm/geometry/2d/validate_circle.hpp
---

## API

- `validate_circle(circle)`: 円の半径が有効か検査する。有効なら何も返さない。

## API別の時間計算量・空間計算量

- `validate_circle`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

負の半径には `std::invalid_argument` を送出する。半径0は有効である。
