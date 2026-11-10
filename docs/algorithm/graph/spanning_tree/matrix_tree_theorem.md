---
title: Matrix-Tree Theorem (行列木定理) [counting_spanning_tree_directed] [counting_spanning_tree_undirected]
documentation_of: ../../../../src/algorithm/graph/spanning_tree/matrix_tree_theorem.hpp
---

行列木定理（Matrix-Tree theorem）で、重み付き全域木および有向全域木の重みの総和を求める。各木の重みは選んだ辺の `weight` の積である。多重辺は別々の辺として数え、自己ループはどの API でも無視する。

各 API には、容量をテンプレート引数で指定する固定長版と、容量を省略して実行時のサイズに合わせた `std::vector` を使う可変長版がある。

```cpp
auto value = count_weighted_undirected_spanning_trees(vertex_count, edges);              // 可変長版
auto fixed_value = count_weighted_undirected_spanning_trees<Mint, 1000>(vertex_count, edges); // 固定長版
```

## 辺

```cpp
template <class T>
struct MatrixTreeEdge { int from; int to; T weight; };
```

無向版では `from` と `to` を辺の両端、有向版では `from -> to` の辺と解釈する。`weight` は負でもよく、符号も含めて体 `T` 上で加算する。

## 固定容量行列と行列式

```cpp
template <class T, std::size_t MAX_SIZE>
using MatrixTreeFixedMatrix = std::array<std::array<T, MAX_SIZE>, MAX_SIZE>;

T matrix_tree_determinant<T, MAX_SIZE>(MatrixTreeFixedMatrix<T, MAX_SIZE> matrix, int n)
T matrix_tree_determinant<T>(std::vector<std::vector<T>> matrix, int n)
```

固定長版は先頭の $n \times n$ 成分の行列式をガウスの消去法で求める。ピボット行を交換するたびに符号を反転する。$n = 0$ の行列式は `T(1)` である。可変長版は `matrix` が $n \times n$ であることを確認し、`gaussian_determinant` で行列式を求める。

## 重み付き無向全域木

```cpp
template <class T, std::size_t MAX_VERTEX = (省略時は可変長)>
T count_weighted_undirected_spanning_trees(int n, const std::vector<MatrixTreeEdge<T>>& edges)
```

辺 $(u, v)$ ごとに無向ラプラシアン

$$
L_{u,u} \mathrel{+}= w,\quad
L_{v,v} \mathrel{+}= w,\quad
L_{u,v} \mathrel{-}= w,\quad
L_{v,u} \mathrel{-}= w
$$

を作り（$w$ は辺の重み）、頂点 0 の行と列を除いた小行列の行列式を返す。非連結なら `T()` を返す。0 頂点のグラフは空の全域木を 1 個もつと数えて `T(1)` を、1 頂点のグラフも `T(1)` を返す。

## `root` から外向きの有向全域木

```cpp
template <class T, std::size_t MAX_VERTEX = (省略時は可変長)>
T count_weighted_directed_out_arborescences(int n, int root, const std::vector<MatrixTreeEdge<T>>& edges)
```

`root` から全頂点へ到達でき、`root` 以外の各頂点の入次数が 1 である外向き有向全域木（out-arborescence）を数える。

辺 `from -> to` ごとに入次数ラプラシアンを

$$
L_{\mathrm{to},\mathrm{to}} \mathrel{+}= w,\qquad
L_{\mathrm{to},\mathrm{from}} \mathrel{-}= w
$$

として作り、`root` の行と列を除いた行列式を返す。API 名の「out」は、辺が `root` から外向きであることを表す。

## `root` へ内向きの有向全域木

```cpp
template <class T, std::size_t MAX_VERTEX = (省略時は可変長)>
T count_weighted_directed_in_arborescences(int n, int root, const std::vector<MatrixTreeEdge<T>>& edges)
```

全頂点から `root` へ到達でき、`root` 以外の各頂点の出次数が 1 である内向き有向全域木（in-arborescence）を数える。

辺 `from -> to` ごとに出次数ラプラシアンを

$$
L_{\mathrm{from},\mathrm{from}} \mathrel{+}= w,\qquad
L_{\mathrm{from},\mathrm{to}} \mathrel{-}= w
$$

として作り、`root` の行と列を除いた行列式を返す。API 名の「in」は、辺が `root` へ内向きであることを表す。

## 時間・空間計算量

$N = n$、$M$ を `edges` の要素数とし、固定長版の容量を $C = \mathrm{MAX\text{-}VERTEX}^2$（`matrix_tree_determinant` では $\mathrm{MAX\text{-}SIZE}^2$）とする。`T` の各演算を $O(1)$ とした計算量である。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `matrix_tree_determinant`（固定長版） | $O(C + N^3)$ | $O(C)$ |
| `count_weighted_undirected_spanning_trees`（固定長版） | $O(C + M + N^3)$ | $O(C)$ |
| `count_weighted_directed_out_arborescences`（固定長版） | $O(C + M + N^3)$ | $O(C)$ |
| `count_weighted_directed_in_arborescences`（固定長版） | $O(C + M + N^3)$ | $O(C)$ |
| 各 API の可変長版 | $O(M + N^3)$ | $O(N^2)$ |

固定長版で容量を $\Theta(N)$ に取る通常の使い方では、各数え上げ API は $O(M + N^3)$ 時間、$O(N^2)$ 空間である。

## 注意点

- `T()` が 0、`T(1)` が 1 を表し、加減乗除、単項マイナス、等値比較ができる体でなければならない。
- 消去法で 0 でないピボットの逆元が存在しなければならない。典型例は素数を法とする modint や有理数である。
- 固定長版は $0 \le n \le$ `MAX_VERTEX` を要求する。固定長版の `matrix_tree_determinant` は $0 \le n \le$ `MAX_SIZE` を要求する。
- すべての辺の端点は $[0, n)$ の範囲内でなければならない。
- 有向版は $0 \le \mathrm{root} < n$ を要求する。このため 0 頂点の有向グラフには有効な `root` がなく、例外を送出する。
- 容量・頂点・`root` の違反時は `runtime_error` を送出する。可変長版の `matrix_tree_determinant` は、行列の形が `n` と合わない場合に `invalid_argument` を送出する。
- 1 頂点の有向グラフでは有効な `root` は 0 であり、自己ループの有無によらず out 版・in 版ともに `T(1)` を返す。
