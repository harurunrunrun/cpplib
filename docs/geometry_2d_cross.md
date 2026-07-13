---
title: 2D Cross Product (二次元外積)
documentation_of: ../src/algorithm/geometry/2d/cross.hpp
---

## API

- `cross(left, right)`: 2ベクトルの外積の符号付きスカラー値を返す。

## API別の時間計算量・空間計算量

- `cross`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

正値は `right` が `left` の反時計回り側にあることを表す。
