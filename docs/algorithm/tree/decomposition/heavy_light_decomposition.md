---
title: Heavy Light Decomposition (HL 分解)
documentation_of: ../../../../src/algorithm/tree/decomposition/heavy_light_decomposition.hpp
---

木を heavy path に分解し、パスや部分木を Euler Tour 上の区間に対応させる。

## API

- `HeavyLightDecomposition(n)`: 頂点数 `n` の空の木を作る。`n < 0` なら `std::runtime_error` を送出する。
- `size()`: 頂点数を返す。
- `add_edge(u, v)`: 無向辺を追加する。
- `build(root = 0)`: `root` を根として分解を構築する。クエリの前に呼ぶ。
- `parent(v)`, `depth(v)`, `subtree(v)`, `head(v)`: 根付き木での親・深さ・部分木サイズ・所属する heavy path の先頭を返す。
- `in(v)`, `out(v)`: Euler Tour 上の頂点 `v` の部分木の半開区間 `[in(v), out(v))` の両端を返す。
- `operator[](k)`: Euler Tour 上の添字 `k` の頂点を返す。
- `lca(u, v)`, `dist(u, v)`: LCA と距離を返す。
- `jump(v, k)`: `v` から根の方向へ `k` 個進んだ頂点を返す。`k < 0` または `k > depth(v)` なら `std::runtime_error` を送出する。
- `is_ancestor(u, v)`: `u` が `v` の祖先（`u == v` を含む）かを返す。
- `path_query(u, v, vertex = true)`: `u` から `v` へのパスを、Euler Tour 上の半開区間 `[l, r)` の列として `u` 側から順に返す。辺に値を持つ場合は `vertex = false` にすると、LCA に対応する頂点を除く。
- `subtree_query(v, vertex = true)`: `v` の部分木に対応する半開区間を返す。`vertex = false` なら `v` 自身を除く。

## 時間計算量

$R$ を `path_query` が返す区間数（$R=O(\log N)$）とする。

- `HeavyLightDecomposition(n)`: $O(N)$
- `size`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `build`: $O(N)$
- `parent`, `depth`, `subtree`, `head`, `in`, `out`, `operator[]`: $O(1)$
- `lca`, `dist`, `jump`: $O(\log(N+1))$
- `is_ancestor`, `subtree_query`: $O(1)$
- `path_query`: 出力する $R$ 個の区間を含めて $O(\log(N+1)+R)=O(\log(N+1))$

## 空間計算量

グラフ、分解の表、出力する区間列を含めて $O(N+R)$ である。

## 注意点

- クエリの前に `build` を呼ぶ必要がある。
- 頂点と `root` は範囲内でなければならない。入力は無向の連結木とする。
- 端点の範囲外、閉路、辺数、連結性の違反時は `std::runtime_error` を送出する。記載した計算量には木の検査を含む。
