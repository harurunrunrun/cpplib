---
title: Exact Geometry Predicate Aggregates (厳密幾何判定集約)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/predicates.hpp
---

向き、包含、平行・直交などの厳密判定APIを一括で読み込む。

## API

- 判定集約ヘッダ: 判定APIを読み込む。

## API別の時間計算量・空間計算量

- 判定集約ヘッダ: 時間 実行時処理なし、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
