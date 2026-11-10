---
title: 3D Finite and Numeric Utilities (三次元幾何の有限値・数値補助API)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/is_finite.hpp
---

## API

`is_finite.hpp` は、引数の型ごとに分割された有限値判定、正規化差分、倍率付きの比較、円・球の検証の API をまとめて include する、後方互換のためのヘッダである。個別の API だけが必要な場合は、対応する個別ヘッダを直接 include する。

## API 別の時間計算量・空間計算量

- 各個別 API の時間計算量: $O(1)$。
- 各個別 API の追加空間計算量: $O(1)$。
- この集約ヘッダ自体は実行時の処理を持たない。

## 注意点

各 API の前提と例外は、それぞれの個別ヘッダのドキュメントに記載する。このヘッダは従来の include を維持するための集約ヘッダであり、新しい関数の定義は持たない。
