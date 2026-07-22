---
title: Counterclockwise Polygon (多角形の反時計回り化)
documentation_of: ../src/algorithm/geometry/2d/point_collection/counterclockwise_polygon.hpp
---

## API

`vector<Point> counterclockwise_polygon(polygon)`

## 引数

周回順の頂点列を値渡しする。

## 戻り値

負の符号付き面積なら順序を反転した頂点列。

## API別の時間計算量・空間計算量

- `counterclockwise_polygon`: 頂点数を $N$ として時間 $O(N)$、戻り値領域 $O(N)$。

## 注意点

面積0の入力順は変更しない。反転時も先頭以外だけを逆順にするため、開始頂点は変更しない。
符号付き面積は先頭頂点を原点とするfanで加算し、各外積と同じ二次元scaleに対する相対許容誤差で符号を判定する。
