---
title: Fast Matrix-Tree Theorem (高速行列木定理)
documentation_of: ../src/algorithm/graph/spanning_tree/fast_matrix_tree_theorem.hpp
---

## 概要

Matrix-Tree theoremと高速行列式を使い、重み付き無向全域木または有向全域木の重み総和を `O(M+N^omega)` で求める。各木の重みは選んだ辺の `weight` の積であり、多重辺は別々に数える。自己loopは無視する。

辺型には既存のMatrix-Tree theorem APIと共通の `MatrixTreeEdge<T>{from, to, weight}` を使う。有向版では `from -> to`、無向版では両端点と解釈する。

## 重み付き無向全域木

~~~cpp
template<class T>
T fast_count_weighted_undirected_spanning_trees(
    int n,
    const std::vector<MatrixTreeEdge<T>>& edges
);
~~~

無向Laplacianから頂点0の行と列を除いたcofactorの行列式を返す。グラフが非連結なら `T()`。0頂点グラフと1頂点グラフは空の全域木を1個と数え、`T(1)` を返す。

## rootから外向きの有向全域木

~~~cpp
template<class T>
T fast_count_weighted_directed_out_arborescences(
    int n,
    int root,
    const std::vector<MatrixTreeEdge<T>>& edges
);
~~~

`root` から全頂点へ到達し、`root` 以外の各頂点の入次数が1であるout-arborescenceの重み総和を返す。辺 `from -> to` に対して入次数Laplacianを

\[
L_{to,to}\mathrel{+}=weight,\qquad
L_{to,from}\mathrel{-}=weight
\]

として作り、`root` の行と列を除く。

## rootへ内向きの有向全域木

~~~cpp
template<class T>
T fast_count_weighted_directed_in_arborescences(
    int n,
    int root,
    const std::vector<MatrixTreeEdge<T>>& edges
);
~~~

全頂点から `root` へ到達し、`root` 以外の各頂点の出次数が1であるin-arborescenceの重み総和を返す。辺 `from -> to` に対して出次数Laplacianを

\[
L_{from,from}\mathrel{+}=weight,\qquad
L_{from,to}\mathrel{-}=weight
\]

として作り、`root` の行と列を除く。

## 時間計算量

`N=n`、`M=edges.size()`、`omega=log_2(7)` とし、`T` の各演算を `O(1)` とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `fast_count_weighted_undirected_spanning_trees` | `O(M+N^omega)` | `O(N^2)` |
| `fast_count_weighted_directed_out_arborescences` | `O(M+N^omega)` | `O(N^2)` |
| `fast_count_weighted_directed_in_arborescences` | `O(M+N^omega)` | `O(N^2)` |

辺をLaplacianへ加える処理が `O(M)`、cofactorの初期化が `O(N^2)`、行列式が `O(N^omega)` である。

## 注意点

- `T{}` が0、`T(1)` が1を表し、加減乗除、単項minus、等値比較ができる正確な体を要求する。
- 全辺の端点は `[0,n)` でなければならない。`n` または端点が範囲外なら `runtime_error`。
- 有向版は `0 <= root < n` を要求し、違反時は `runtime_error`。したがって0頂点の有向グラフには使えない。
- 辺の重みは負でもよい。体上の値として加算する。
