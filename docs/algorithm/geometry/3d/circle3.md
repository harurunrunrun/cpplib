---
title: 3D Circle Type (三次元円型)
documentation_of: ../../../../src/algorithm/geometry/3d/circle3.hpp
---

## API

- `Circle3{center, normal, radius}`: 中心、平面法線、半径で円を表すaggregate型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域 $O(1)$。

## 注意点

法線は零ベクトルであってはならず、半径は非負でなければならない。
