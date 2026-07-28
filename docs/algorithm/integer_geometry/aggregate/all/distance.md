---
title: Exact Geometry Distance Aggregates (厳密幾何距離集約)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/distance.hpp
---

距離、最近点対、直径の厳密APIを一括で読み込む。

## API

- 距離集約ヘッダ: 距離APIを読み込む。

## API別の時間計算量・空間計算量

- 距離集約ヘッダ: 時間 実行時処理なし、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
