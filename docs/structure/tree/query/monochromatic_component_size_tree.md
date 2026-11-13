---
title: Monochromatic Component Size Tree (同色連結成分サイズ) [QTREE6]
documentation_of: ../../../../src/structure/tree/query/monochromatic_component_size_tree.hpp
---

2 色で塗られた木の頂点の色を反転し、同色連結成分の頂点数を返す。
`DynamicMonochromaticTree` に `AddMonoid<int>` と全頂点の値 1 を与えた薄いラッパーである。

## API

### `MonochromaticComponentSizeTree<MAX_SIZE>(n)`

頂点数 `n` で、全頂点が同じ初期色（`false`）の構造を作る。`MAX_SIZE` は省略でき、デフォルト値 `-1` では可変長版になる。
時間計算量は固定長版で $O(\texttt{MAX_SIZE})$、可変長版で $O(n)$ である。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は $O(1)$ である。

### `build()`

索引を構築する。時間計算量は $O(n)$ である。

### `toggle(vertex)`

頂点の色を反転し、反転後の色を返す。時間計算量は構築前は $O(1)$、構築後は $O(\log n)$ である。

### `component_size(vertex)`

`vertex` を含む同色連結成分の頂点数を返す。時間計算量は $O(\log^2 n)$ である。

## 注意点

- 頂点数は `MAX_SIZE` 以下で、辺集合は連結な木でなければならない。
- `build` 前の `component_size`、範囲外の頂点、木でない入力では `runtime_error` を送出する。
- 容量を指定した固定配列版（`MonochromaticComponentSizeTree<N>`）と可変長版（`MonochromaticComponentSizeTree<>`）は同じヘッダで利用できる。
