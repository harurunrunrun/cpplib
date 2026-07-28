---
title: Exact Polygon Aggregates (厳密多角形集約)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/polygon.hpp
---

多角形の面積、内外判定、基本判定APIを一括で読み込む。

## API

- 多角形集約ヘッダ: 多角形APIを読み込む。

## API別の時間計算量・空間計算量

- 多角形集約ヘッダ: 時間 実行時処理なし、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
