---
title: 2D Circle (二次元円)
documentation_of: ../../../../src/algorithm/geometry/2d/circle.hpp
---

中心と半径で円を表す。

## API

- `Circle{center, radius}`: 中心と半径から構築する。

## API別の時間計算量・空間計算量

- 構築とfield参照は時間 $O(1)$、追加領域 $O(1)$。

## 注意点

半径の有限性・非負性は、必要に応じて `validate_circle` で検査する。
