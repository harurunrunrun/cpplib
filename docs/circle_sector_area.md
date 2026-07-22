---
title: Circle Sector Area (扇形面積)
documentation_of: ../src/algorithm/geometry/2d/scalar/circle_sector_area.hpp
---

## API

- `circle_sector_area(circle, central_angle)`: 指定中心角の扇形面積を返す。

## 引数

`circle` は非負半径の円、`central_angle` はrad単位の有限な非負角である。

## 戻り値

$r^2\theta/2$ を `long double` で返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

負半径、負角、非有限角には `std::invalid_argument` を送出する。
