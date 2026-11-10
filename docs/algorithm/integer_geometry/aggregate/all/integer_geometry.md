---
title: Exact Integer Geometry (厳密整数幾何)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/integer_geometry.hpp
---

整数と有理数で誤差なく計算する二次元幾何ライブラリの全体を読み込む。

## API

このヘッダ自体は関数を定義せず、次のヘッダを読み込む。

- `aggregate/all/convex.hpp`
- `aggregate/all/distance.hpp`
- `aggregate/all/intersection.hpp`
- `aggregate/all/lattice.hpp`
- `aggregate/all/polygon.hpp`
- `aggregate/all/predicates.hpp`
- `aggregate/all/types.hpp`

## API別の時間計算量・空間計算量

- 集約ヘッダ自体: 実行時の処理はなく、追加領域は $O(1)$。

## 注意点

計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
