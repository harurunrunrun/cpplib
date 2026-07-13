---
title: Matrix-Tree Theorem
documentation_of: ../src/algorithm/graph/matrix_tree_theorem.hpp
---

Matrix-Tree theoremで、重み付き全域木および有向全域木の重み総和を求める。各木の重みは、選んだ辺の `weight` の積である。多重辺は別々の辺として数え、自己loopはどのAPIでも無視する。

# 辺

~~~cpp
MatrixTreeEdge<T>{from, to, weight}
~~~

無向版では `from` と `to` を辺の両端、有向版では `from -> to` と解釈する。`weight` は負でもよく、符号を含めて体 `T` 上で加算する。

# 固定容量行列と行列式

~~~cpp
MatrixTreeFixedMatrix<T, MAX_SIZE>
matrix_tree_determinant<T, MAX_SIZE>(matrix, n)
~~~

`MatrixTreeFixedMatrix` は `std::array<std::array<T, MAX_SIZE>, MAX_SIZE>` の別名。先頭 `n x n` 成分の行列式をGaussian eliminationで求める。pivot行を交換するたびに符号を反転する。`n == 0` の行列式は `T(1)`。

# 重み付き無向全域木

~~~cpp
count_weighted_undirected_spanning_trees<T, MAX_VERTEX>(n, edges)
~~~

無向Laplacian

\[
L_{u,u} \mathrel{+}= w,\quad
L_{v,v} \mathrel{+}= w,\quad
L_{u,v} \mathrel{-}= w,\quad
L_{v,u} \mathrel{-}= w
\]

から1行1列を除いたcofactorの行列式を返す。非連結なら `T()`。0頂点グラフは空の全域木を1個と数えて `T(1)`、1頂点グラフも `T(1)` を返す。

# rootから外向きの有向全域木

~~~cpp
count_weighted_directed_out_arborescences<T, MAX_VERTEX>(
    n, root, edges
)
~~~

`root` から全頂点へ到達でき、`root` 以外の各頂点の入次数が1となるout-arborescenceを数える。

辺 `from -> to` に対し、入次数Laplacianを

\[
L_{to,to} \mathrel{+}= w,\qquad
L_{to,from} \mathrel{-}= w
\]

として作り、`root` の行と列を除いた行列式を返す。API名の「out」は辺がrootから外向きであることを表す。

# rootへ内向きの有向全域木

~~~cpp
count_weighted_directed_in_arborescences<T, MAX_VERTEX>(
    n, root, edges
)
~~~

全頂点から `root` へ到達でき、`root` 以外の各頂点の出次数が1となるin-arborescenceを数える。

辺 `from -> to` に対し、出次数Laplacianを

\[
L_{from,from} \mathrel{+}= w,\qquad
L_{from,to} \mathrel{-}= w
\]

として作り、`root` の行と列を除いた行列式を返す。API名の「in」は辺がrootへ内向きであることを表す。

# 時間・空間計算量

`N=n`、`M=edges.size()`、`C=MAX_VERTEX^2` とする。`T` の各演算を `O(1)` とした計算量である。

| API | 時間計算量 | 追加空間 |
| --- | --- | --- |
| `matrix_tree_determinant<T, MAX_SIZE>(matrix, n)` | `O(MAX_SIZE^2 + N^3)` | `O(MAX_SIZE^2)` |
| `count_weighted_undirected_spanning_trees` | `O(C + M + N^3)` | `O(C)` |
| `count_weighted_directed_out_arborescences` | `O(C + M + N^3)` | `O(C)` |
| `count_weighted_directed_in_arborescences` | `O(C + M + N^3)` | `O(C)` |

固定容量を `MAX_VERTEX = Theta(N)` と取る通常の利用では、各count APIは `O(M+N^3)` 時間、`O(N^2)` 空間である。

# 前提・例外

- `T()` が0、`T(1)` が1を表し、加減乗除、単項minus、等値比較ができる体を要求する。
- eliminationで非0pivotの逆元が存在しなければならない。典型例は素数modのmodintや有理数である。
- `0 <= n <= MAX_VERTEX` を要求する。`matrix_tree_determinant` は `0 <= n <= MAX_SIZE` を要求する。
- 全辺の端点は `[0,n)` でなければならない。
- 有向版は `0 <= root < n` を要求する。このため0頂点の有向グラフには有効なrootがなく、例外を送出する。
- 容量、頂点、rootの違反時は `runtime_error`。
- 1頂点の有向グラフでは有効なrootを0とし、自己loopの有無によらずout/inともに `T(1)`。
