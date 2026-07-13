---
title: Lowest Common Ancestor
documentation_of: ../src/algorithm/tree/lca.hpp
---

二分 lifting による LCA。

## 関数

- `add_edge(u, v)`
- `build(root = 0)`
- `lca(u, v)`
- `dist(u, v)`
- `jump(v, k)`
    - `v` から根方向へ `k` 個進んだ頂点。
- `parent(v)`, `depth(v)`

## 時間計算量

- `LowestCommonAncestor(n)`: $O(N\log(N+1))$
- `size`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `build`: $O(N\log(N+1))$
- `parent`, `depth`: $O(1)$
- `jump`, `lca`, `dist`: $O(\log(N+1))$

constructorはbinary lifting tableを確保・初期化し、`build` は木の検査とtable構築を行う。

## 空間計算量

- graphとbinary lifting tableを含めて $O(N\log(N+1))$

## API契約・前提・例外

`parent(root) == -1`, `depth(root) == 0`。`jump(v,k)` は親方向へちょうどk辺進み、`dist` は辺数を返す。辺追加後にbuildする。負のn/k、範囲外頂点/root、`k > depth(v)`、非木入力で `runtime_error`。

頂点とrootは範囲内でなければならない。空木を明記した場合を除き、入力は無向の連結木とする。端点範囲、閉路、辺数、連結性の違反時は `runtime_error` を送出する。記載した計算量には木の検査とResultの構築を含む。
