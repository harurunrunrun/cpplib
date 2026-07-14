---
title: Circle-Line Intersection Count (円と直線の交点数)
documentation_of: ../src/algorithm/geometry/2d/circle_line_intersection_count.hpp
---

## API

- `circle_line_intersection_count(circle, line)`: 円周と無限直線の異なる交点数を返す。

## 引数

`circle` は非負半径の円、`line` は非退化直線である。

## 戻り値

0、1、2のいずれかを返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

接するときは1。負半径または退化直線には `std::invalid_argument` を送出する。
