---
title: 2D Circumcenter (二次元外心)
documentation_of: ../src/algorithm/geometry/2d/circumcenter.hpp
---

## API

- `circumcenter(first, second, third)`: 3点を通る円の中心を返す。

## API別の時間計算量・空間計算量

- `circumcenter`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

3点が共線で一意な外心を持たない場合は `std::invalid_argument` を送出する。
