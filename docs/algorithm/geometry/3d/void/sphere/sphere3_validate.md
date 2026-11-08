---
title: Sphere3 Validation (三次元球の検証)
documentation_of: ../../../../../../src/algorithm/geometry/3d/void/sphere/sphere3_validate.hpp
---

## API

- `geometry3d_validate(sphere)`: 中心・半径が有限であることと、半径が非負であることを検査する。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

条件に違反した場合は `std::invalid_argument` を送出する。
