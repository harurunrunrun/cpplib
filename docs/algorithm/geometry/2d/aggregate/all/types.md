---
title: 2D Geometry Types (二次元幾何型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/types.hpp
---

`Point`、`Line`、`Segment`、`Circle` をまとめて読み込む集約ヘッダ。

## API

- `#include "types.hpp"`: 二次元幾何の基本型をすべて読み込む。

## API別の時間計算量・空間計算量

- 型の読み込み自体に実行時計算量と追加領域はない。

## 注意点

`Line` と `Segment` は異なる型であり、無限直線と閉線分を型で区別する。
