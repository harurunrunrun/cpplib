---
title: 3D Finite and Numeric Utilities (三次元幾何の有限値・数値補助API)
documentation_of: ../../../../src/algorithm/geometry/3d/is_finite.hpp
---

## API

`is_finite.hpp` は、引数型ごとに分割された有限値判定、正規化差、倍率付き比較、円・球の検証APIをまとめてincludeする後方互換headerである。個別APIだけが必要な場合は対応するleaf headerを直接includeする。

## API別の時間計算量・空間計算量

- 各leaf APIの時間計算量: $O(1)$
- 各leaf APIの追加空間計算量: $O(1)$
- このaggregator自体は実行時処理を持たない。

## 注意点

APIの前提・例外は各leaf documentに記載する。このheaderは従来のincludeを維持するためのaggregatorであり、新しい関数定義は持たない。
