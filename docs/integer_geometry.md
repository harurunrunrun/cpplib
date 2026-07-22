---
title: Integer Geometry Aggregate Header (整数幾何一括ヘッダ)
documentation_of: ../src/algorithm/integer_geometry/integer_geometry.hpp
---

## API

整数幾何の基本型、厳密判定、有理交点、多角形、凸包、距離APIをすべて読み込む一括ヘッダ。

## API別の時間計算量・空間計算量

ヘッダの読み込み自体に実行時の計算量・領域消費はない。各APIの計算量は対応する個別ドキュメントに従う。

## 注意点

すべて `integer_geometry` 名前空間に属する。GCC 13の `__int128_t` / `__uint128_t` を使用し、浮動小数点数には依存しない。
