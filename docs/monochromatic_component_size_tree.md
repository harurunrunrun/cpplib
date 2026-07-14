---
title: Monochromatic Component Size Tree (同色連結成分サイズ) [QTREE6]
documentation_of: ../src/structure/tree/monochromatic_component_size_tree.hpp
---

2色で塗られた木の頂点色を反転し、同色連結成分の頂点数を返す。

## API

### `MonochromaticComponentSizeTree<MAX_SIZE>(n)`

全頂点を同じ初期色にする。時間計算量は $O(n)$。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は償却 $O(1)$。

### `build()`

動的木索引を構築する。時間計算量は $O(n\log n)$。

### `toggle(vertex)`

頂点色を反転する。時間計算量は償却 $O(\log n)$。

### `component_size(vertex)`

同色連結成分の頂点数を返す。時間計算量は償却 $O(\log n)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下、辺集合は連結な木でなければならない。
- `build` 前の操作や範囲外頂点では `runtime_error`。
