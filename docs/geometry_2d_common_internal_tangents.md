---
title: 2D Common Internal Tangents (二次元円の共通内接線)
documentation_of: ../src/algorithm/geometry/2d/shape/common_internal_tangents.hpp
---

## API

- `common_internal_tangents(first, second)`: 2円の共通内接線を返す。

## 引数

`first`, `second` は非負半径の円である。

## 戻り値

異なる共通内接線を0〜2本返す。

## API別の時間計算量・空間計算量

時間・出力・追加領域 $O(1)$。

## 注意点

一致円では無限本あるため `std::domain_error` を送出する。
