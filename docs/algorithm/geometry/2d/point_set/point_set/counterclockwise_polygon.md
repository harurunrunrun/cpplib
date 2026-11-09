---
title: Counterclockwise Polygon (多角形の反時計回り化)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set/counterclockwise_polygon.hpp
---

## API

`std::vector<Point> counterclockwise_polygon(std::vector<Point> polygon)`

## 引数

周回順に並んだ頂点列を値渡しで受け取る。

## 戻り値

符号付き面積が負なら順序を反転した頂点列を、そうでなければ入力のままの頂点列を返す。

## API別の時間計算量・空間計算量

- `counterclockwise_polygon`: 頂点数を $N$ として時間 $O(N)$、戻り値の領域 $O(N)$。

## 注意点

- 面積が $0$ と判定された入力（頂点数 $3$ 未満を含む）は順序を変更しない。
- 反転するときも先頭以外の頂点だけを逆順にするため、開始頂点は変わらない。
- 符号付き面積は先頭頂点を原点とする扇形の三角形分割で加算し、各外積と同じ次元のスケール（辺ベクトルの長さの積の和）に対する相対許容誤差で符号を判定する。
