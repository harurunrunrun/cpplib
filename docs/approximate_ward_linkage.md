---
title: Ward Linkage Hierarchical Clustering (Ward法階層クラスタリング)
documentation_of: ../src/approximate/clustering/ward_linkage.hpp
---

Ward法で凝集型階層clusterを構築する。

## WardMerge / WardClusteringResult

葉は `0 .. N-1`、merge `i` が作るnodeは `N+i` である。

- `WardMerge::left`, `WardMerge::right`: mergeする左右のnode番号。
- `WardMerge::squared_error_increase`: mergeによるcluster内平方誤差の増分。
- `WardMerge::size`: merge後のclusterに含まれる点数。
- `WardClusteringResult::merges`: 実行順に並んだ $N-1$ 個のmerge。

各member参照の時間計算量・追加空間計算量は $O(1)$、`merges` 全体の列挙は時間計算量 $O(N)$、追加空間計算量 $O(1)$ である。

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

## 注意点

座標列または距離callbackは、各APIで示した次元・有限性・非負性の条件を満たす必要がある。初期値や入力順に依存する手法では、明記した場合を除いて一意な分割や大域最適性を保証しない。
