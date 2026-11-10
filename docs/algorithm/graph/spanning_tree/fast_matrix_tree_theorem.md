---
title: Fast Matrix-Tree Theorem (高速行列木定理)
documentation_of: ../../../../src/algorithm/graph/spanning_tree/fast_matrix_tree_theorem.hpp
---

## 概要

行列木定理（Matrix-Tree theorem）と高速な行列式計算を使い、重み付き無向全域木または有向全域木の重みの総和を $O(M + N^{\omega})$ で求める。各木の重みは選んだ辺の `weight` の積であり、多重辺は別々に数える。自己ループは無視する。

辺の型には、既存の行列木定理の API と共通の `MatrixTreeEdge<T>{from, to, weight}` を使う。有向版では `from -> to` の辺、無向版では両端点を結ぶ辺と解釈する。

## 重み付き無向全域木

```cpp
template <class T>
T fast_count_weighted_undirected_spanning_trees(
    int n,
    const std::vector<MatrixTreeEdge<T>>& edges
);
```

無向ラプラシアンから頂点 0 の行と列を除いた小行列の行列式を返す。グラフが非連結なら `T()` を返す。0 頂点のグラフと 1 頂点のグラフは空の全域木を 1 個もつと数え、`T(1)` を返す。

## `root` から外向きの有向全域木

```cpp
template <class T>
T fast_count_weighted_directed_out_arborescences(
    int n,
    int root,
    const std::vector<MatrixTreeEdge<T>>& edges
);
```

`root` から全頂点へ到達し、`root` 以外の各頂点の入次数が 1 である外向き有向全域木（out-arborescence）の重みの総和を返す。辺 `from -> to` ごとに入次数ラプラシアンを

$$
L_{\mathrm{to},\mathrm{to}} \mathrel{+}= w, \qquad
L_{\mathrm{to},\mathrm{from}} \mathrel{-}= w
$$

として作り（$w$ は辺の重み）、`root` の行と列を除く。

## `root` へ内向きの有向全域木

```cpp
template <class T>
T fast_count_weighted_directed_in_arborescences(
    int n,
    int root,
    const std::vector<MatrixTreeEdge<T>>& edges
);
```

全頂点から `root` へ到達し、`root` 以外の各頂点の出次数が 1 である内向き有向全域木（in-arborescence）の重みの総和を返す。辺 `from -> to` ごとに出次数ラプラシアンを

$$
L_{\mathrm{from},\mathrm{from}} \mathrel{+}= w, \qquad
L_{\mathrm{from},\mathrm{to}} \mathrel{-}= w
$$

として作り、`root` の行と列を除く。

## 計算量

$N = n$、$M$ を `edges` の要素数、$\omega = \log_2 7$ とし、`T` の各演算を $O(1)$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `fast_count_weighted_undirected_spanning_trees` | $O(M + N^{\omega})$ | $O(N^2)$ |
| `fast_count_weighted_directed_out_arborescences` | $O(M + N^{\omega})$ | $O(N^2)$ |
| `fast_count_weighted_directed_in_arborescences` | $O(M + N^{\omega})$ | $O(N^2)$ |

辺をラプラシアンに加える処理が $O(M)$、小行列の初期化が $O(N^2)$、行列式が $O(N^{\omega})$ である。

## 注意点

- `T{}` が 0、`T(1)` が 1 を表し、加減乗除、単項マイナス、等値比較ができる正確な体でなければならない。
- すべての辺の端点は $[0, n)$ の範囲内でなければならない。`n` が負、または端点が範囲外なら `runtime_error` を送出する。
- 有向版は $0 \le \mathrm{root} < n$ を要求し、違反時は `runtime_error` を送出する。したがって 0 頂点の有向グラフには使えない。
- 辺の重みは負でもよい。体の値としてそのまま加算する。
