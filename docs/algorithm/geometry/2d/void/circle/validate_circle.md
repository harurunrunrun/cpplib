---
title: 2D Circle Validation (二次元円の検査)
documentation_of: ../../../../../../src/algorithm/geometry/2d/void/circle/validate_circle.hpp
---

## API

- `validate_circle(circle)`: 円の中心座標と半径が有効か検査する。有効なら何もせずに戻る（戻り値は `void`）。

## API 別の時間計算量・空間計算量

- `validate_circle`: 時間 $O(1)$、追加空間 $O(1)$。

## 注意点

- 中心座標または半径が非有限値（NaN・無限大）なら `std::invalid_argument` を送出する。
- 半径が負なら `std::invalid_argument` を送出する。
- 有限な半径 0 は有効である。
