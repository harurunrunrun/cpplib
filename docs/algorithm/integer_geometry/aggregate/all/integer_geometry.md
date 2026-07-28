---
title: Exact Integer Geometry (厳密整数幾何)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/integer_geometry.hpp
---

整数と有理数で誤差なく計算する二次元幾何ライブラリ全体を読み込む。

## API

- 全体集約ヘッダ: すべての公開APIを読み込む。

## API別の時間計算量・空間計算量

- 全体集約ヘッダ: 時間 実行時処理なし、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
