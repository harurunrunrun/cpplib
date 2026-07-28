---
title: Exact Lattice Geometry Aggregates (厳密格子幾何集約)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/lattice.hpp
---

格子多角形の面積・境界点数・内部点数APIを一括で読み込む。

## API

- 格子幾何集約ヘッダ: 格子幾何APIを読み込む。

## API別の時間計算量・空間計算量

- 格子幾何集約ヘッダ: 時間 実行時処理なし、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
