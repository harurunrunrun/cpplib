---
title: Ward Linkage Hierarchical Clustering (Ward法階層クラスタリング)
documentation_of: ../../../src/approximate/clustering/ward_linkage.hpp
---

Ward 法で凝集型の階層的クラスタリングを行う。

## `WardMerge` / `WardClusteringResult`

葉は `0` から `N-1`、`i` 番目のマージで作られるノードは `N+i` である。

- `WardMerge::left`, `WardMerge::right`: マージする左右のノード番号。
- `WardMerge::squared_error_increase`: マージによるクラスタ内平方誤差の増分。
- `WardMerge::size`: マージ後のクラスタに含まれる点数。
- `WardClusteringResult::merges`: 実行順に並んだ $N-1$ 個のマージ。

各メンバの参照は時間計算量・追加空間計算量とも $O(1)$、`merges` 全体の列挙は時間計算量 $O(N)$、追加空間計算量 $O(1)$ である。

## `ward_linkage`

~~~cpp
WardClusteringResult ward_linkage(const DensePoints<Real>& points)
~~~

各ステップで

$$
\frac{|A||B|}{|A|+|B|}\|\mu_A-\mu_B\|^2
$$

が最小となる活性なクラスタの対をマージする。同値の場合はノード番号の辞書順で選ぶ。貪欲な階層化であり、指定したクラスタ数に対する平方誤差の大域最小は保証しない。

- 時間計算量: $O(N^3D)$
- 追加空間計算量: $O(ND)$

## 注意点

- 空の入力と 1 点の入力では、空のマージ列を返す。
- 全点は同じ正の次元と有限の座標を持たなければならない。
- 距離の二乗やマージのコストを表せない場合は `std::overflow_error`、点数が大きすぎてノード番号を表せない場合は `std::length_error` を送出する。
