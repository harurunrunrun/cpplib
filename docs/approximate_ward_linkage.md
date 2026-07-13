---
title: Ward Linkage Hierarchical Clustering (Ward法階層クラスタリング)
documentation_of: ../src/approximate/clustering/ward_linkage.hpp
---

Ward法で凝集型階層clusterを構築する。

## WardMerge / WardClusteringResult

葉は `0 .. N-1`、merge `i` が作るnodeは `N+i` である。`WardMerge` は左右のnode、mergeによるcluster内平方誤差の増分 `squared_error_increase`、merge後の点数 `size` を持つ。

## ward_linkage

~~~cpp
WardClusteringResult ward_linkage(const DensePoints<Real>& points)
~~~

各stepで

$$\frac{|A||B|}{|A|+|B|}\|\mu_A-\mu_B\|^2$$

が最小のactive cluster対をmergeする。同値ではnode番号の辞書順を選ぶ。これはgreedyな階層化であり、指定cluster数に対する大域最小平方誤差を保証しない。

- 時間計算量: $O(N^3D)$
- 追加空間計算量: $O(ND)$

空入力と1点入力ではmerge列を空で返す。全点は同じ正の次元と有限座標を持たなければならない。距離二乗またはmerge costを表せない場合は `std::overflow_error` を送出する。
