---
title: Sphere3 Validation (三次元球の検証)
documentation_of: ../../../../src/algorithm/geometry/3d/sphere3_validate.hpp
---

## API

- `geometry3d_validate(sphere)` は中心・半径の有限性と半径の非負性を検査する。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

契約違反時は `std::invalid_argument` を送出する。
