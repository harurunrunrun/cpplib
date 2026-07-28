---
title: Exact Geometry Intersection Aggregates (厳密幾何交差集約)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/intersection.hpp
---

直線・線分の交差判定と交点計算APIを一括で読み込む。

## API

- 交差集約ヘッダ: 交差APIを読み込む。

## API別の時間計算量・空間計算量

- 交差集約ヘッダ: 時間 実行時処理なし、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
