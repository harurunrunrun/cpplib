---
title: Heavy Light Decomposition (HL分解) [ABC294_G] [GRASSPLA] [QTREE3] [vertex_add_path_sum]
documentation_of: ../src/algorithm/tree/heavy_light_decomposition.hpp
---

木を heavy path に分解し、パス・部分木を Euler Tour 上の区間に移す。

`path_query(u, v, vertex)` は `[l, r)` の列を返す。辺に値を持つ場合は `vertex = false` にして、LCA に対応する頂点を除く。

## 関数

- `add_edge(u, v)`
- `build(root = 0)`
- `lca(u, v)`
- `dist(u, v)`
- `jump(v, k)`
    - `v` から根方向へ `k` 個進んだ頂点。
- `is_ancestor(u, v)`
- `path_query(u, v, vertex = true)`
- `subtree_query(v, vertex = true)`
- `in(v)`, `out(v)`, `operator[](k)`

## 時間計算量

$R$ を `path_query` が返す区間数（$R=O(\log N)$）とする。

- `HeavyLightDecomposition(n)`: $O(N)$
- `size`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `build`: $O(N)$
- `parent`, `depth`, `subtree`, `head`, `in`, `out`, `operator[]`: $O(1)$
- `lca`, `dist`, `jump`: $O(\log(N+1))$
- `is_ancestor`, `subtree_query`: $O(1)$
- `path_query`: $O(\log(N+1)+R)=O(\log(N+1))$。$R$ 区間の出力を含む

## 空間計算量

- graph、分解table、出力区間列を含めて $O(N+R)$

## 注意点

`parent/depth/subtree/head/in/out` は根付き木metadata、`operator[]` はEuler indexから頂点への写像。`path_query` は半開区間列、`subtree_query(v,false)` はv自身を除く。query前にbuildする。jump範囲や非木入力の違反で `runtime_error`。

頂点とrootは範囲内でなければならない。空木を明記した場合を除き、入力は無向の連結木とする。端点範囲、閉路、辺数、連結性の違反時は `runtime_error` を送出する。記載した計算量には木の検査とResultの構築を含む。
