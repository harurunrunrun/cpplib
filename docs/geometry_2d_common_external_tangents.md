---
title: 2D Common External Tangents (二次元円の共通外接線)
documentation_of: ../src/algorithm/geometry/2d/common_external_tangents.hpp
---

## API

- `common_external_tangents(first, second)`: 2円の共通外接線を返す。

## 引数

`first`, `second` は非負半径の円である。

## 戻り値

異なる共通外接線を0〜2本返す。

## API別の時間計算量・空間計算量

時間・出力・追加領域 $O(1)$。

## 注意点

一致円では無限本あるため `std::domain_error` を送出する。
