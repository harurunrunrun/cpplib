---
title: Exact Rational Geometry (厳密有理数幾何)
documentation_of: ../../src/integer_geometry/integer_geometry.hpp
---

任意精度整数と既約有理数で表現する二次元幾何および格子多角形APIをまとめて読み込む集約ヘッダ。

## API

- `#include "integer_geometry.hpp"`: `number`、`2d`、`lattice` の公開ヘッダをすべて読み込む。

## API別の時間計算量・空間計算量

- ヘッダの読み込み自体に実行時計算量と追加領域はない。各APIの計算量は対応する個別docsに従う。

## 注意点

浮動小数点数を使わないため幾何判定に許容誤差はない。座標値のbit長に応じて任意精度演算の実行時間と領域が増える。
