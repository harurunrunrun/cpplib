---
title: Tree Contour Fenwick Index (重み付き木の距離帯 Fenwick 索引) [vertex_add_range_contour_sum_on_tree] [vertex_get_range_contour_add_on_tree]
documentation_of: ../../../../src/structure/tree/centroid/tree_contour_fenwick.hpp
---

`TreeContourFenwick<T,Distance,MAX_SIZE,RangeAdd=false>(values,edges)` を構築する。
`edges` は `(u,v,非負の距離)` の `vector<tuple<int,int,Distance>>`。

- `RangeAdd=false`: `point_add(v,x)` と `contour_sum(v,l,r)`。後者は距離が `[l,r)` の頂点値の和。
- `RangeAdd=true`: `contour_add(v,l,r,x)` と `point_get(v)`。前者は距離が `[l,r)` の全頂点へ加算。
- `size()` は頂点数。

## 時間計算量

重心分解と距離座標圧縮で構築 $O(N\log^2 N)$、各更新・照会 $O(\log^2 N)$。
使用空間は $O(\mathtt{MAX\_SIZE}\log\mathtt{MAX\_SIZE}+N\log N)$。

## 注意点

`RangeAdd` の二つのモードを同じインスタンスで混用しない。モードに対応しない操作は呼び出せない。
零重みの辺、負の距離帯端点、空の距離帯を許す。距離帯は `l<=r`。
距離の加算と端点から距離を引いた値が `Distance` に収まること。
木の検査および距離の overflow 検査は内部の距離索引が行う。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = TreeContourFenwick<T, Distance>;
using Fixed = TreeContourFenwick<T, Distance, N>;
```

可変長版は実行時の要素数・頂点数に応じて配列を確保する。ノードを追加する構造では伸長費用が償却 $O(1)$、既存の操作計算量を維持する。
領域は使用ノード数に比例し、木の距離索引など既存の段別領域は上記と同じ漸近量。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
