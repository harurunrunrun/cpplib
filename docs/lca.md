---
title: Lowest Common Ancestor (最小共通祖先) [ABC014_4] [GRL_5_C] [lca]
documentation_of: ../src/algorithm/tree/lca.hpp
---

根付き木上の最小共通祖先、頂点間距離、祖先方向への移動を扱う。
Euler tour の深さ列に range minimum query の sparse table を構築するため、
最小共通祖先とそれを使う距離問い合わせは最悪 $O(1)$ である。
祖先方向への移動のために binary lifting table も保持する。

## API

### `LowestCommonAncestor(n = 0)`

頂点集合を $[0,n)$ とする空の無向グラフを構築する。

### `size()`

頂点数 $n$ を返す。

### `add_edge(u, v)`

無向辺 $\{u,v\}$ を追加する。追加後は `build` を再実行する必要がある。

### `build(root = 0)`

追加済みの辺が連結な木をなすことを検査し、`root` を根として親、深さ、
Euler tour RMQ、binary lifting table を構築する。同じ木を別の根で再構築してよい。
空の木では何も構築せず正常終了する。

### `parent(v)`

根付き木における `v` の親を返す。根では $-1$ を返す。

### `depth(v)`

根から `v` までの辺数を返す。

### `jump(v, k)`

`v` から親方向へちょうど $k$ 辺進んだ頂点を返す。$k=0$ なら `v` を返す。

### `lca(u, v)`

`u` と `v` の最小共通祖先を返す。

### `dist(u, v)`

`u` と `v` を結ぶ単純パスの辺数を返す。

### `path_vertex_count(u, v)`

`u` と `v` を結ぶ単純パス上の頂点数を、両端を含めて返す。

## 時間計算量

- `LowestCommonAncestor(n)`: $O(N\log(N+1))$
- `size()`: $O(1)$
- `add_edge(u, v)`: 償却 $O(1)$
- `build`: $O(N\log(N+1))$
- `parent(v)`, `depth(v)`: $O(1)$
- `jump(v, k)`: $O(\log(N+1))$
- `lca(u, v)`, `dist(u, v)`, `path_vertex_count(u, v)`: $O(1)$

## 空間計算量

隣接リスト、Euler tour の sparse table、binary lifting table を含めて
$O(N\log(N+1))$。

## 注意点

`parent`、`depth`、`jump`、`lca`、`dist`、
`path_vertex_count` の前に、最後の辺追加より後で `build` を完了させる。
頂点と根は範囲内でなければならず、空でない入力は自己ループや多重辺を含まない
無向連結木でなければならない。

負の $n$ または $k$、範囲外の頂点または根、$k>\mathrm{depth}(v)$、
`build` 前の問い合わせ、辺数・閉路・連結性に違反する入力では
`std::runtime_error` を送出する。
