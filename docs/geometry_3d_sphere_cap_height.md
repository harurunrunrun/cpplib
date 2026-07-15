---
title: 3D Sphere Cap Height (三次元球冠の高さ)
documentation_of: ../src/algorithm/geometry/3d/sphere_cap_height.hpp
---

## API

- `sphere_cap_height(sphere, cutting_plane, side)`: `side` が1なら平面法線側、-1なら反対側にある球部分を球冠とみなし、その高さを返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

高さは $[0,2r]$ にclampされるため、球と交わらない平面にも対応する。`side` は -1 または1でなければならず、負半径または零法線には `std::invalid_argument` を送出する。
