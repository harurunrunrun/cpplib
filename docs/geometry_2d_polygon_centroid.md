---
title: 2D Polygon Centroid (二次元多角形の面積重心)
documentation_of: ../src/algorithm/geometry/2d/polygon_centroid.hpp
---

## API

`Point polygon_centroid(polygon)`

## 引数

単純多角形の周回順頂点列を渡す。

## 戻り値

一様な面密度を持つ多角形の重心。

## API別の時間計算量・空間計算量

- `polygon_centroid`: 頂点数を $N$ として時間 $O(N)$、追加領域 $O(1)$。

## 注意点

3頂点未満、または符号付き面積が相対許容誤差内で0なら
`std::invalid_argument`。時計回り入力にも対応する。先頭頂点を原点とした三角形分割で
面積と局所加重和を計算し、最後に先頭頂点を戻すため、大きな平行移動や凹多角形の
正負面積の相殺による精度低下を抑える。座標と結果は有限な `long double` の範囲に
収まる必要がある。
