---
title: Circle-Segment Intersection Count (円と線分の交点数)
documentation_of: ../src/algorithm/geometry/2d/circle_segment_intersection_count.hpp
---

## API

- `circle_segment_intersection_count(circle, segment)`: 円周と閉線分の異なる交点数を返す。

## 引数

`circle` は非負半径の円、`segment` は退化してもよい閉線分である。

## 戻り値

0、1、2のいずれかを返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

端点接触を含む。負半径には `std::invalid_argument` を送出する。
