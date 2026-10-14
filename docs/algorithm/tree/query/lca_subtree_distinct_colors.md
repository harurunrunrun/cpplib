---
title: LCA Subtree Distinct Colors (LCA部分木色数) [ADAORANG]
documentation_of: ../../../../src/algorithm/tree/query/lca_subtree_distinct_colors.hpp
---

2頂点のLCAを根とする部分木に現れる異なる色数を返す静的構造である。

## API

### `LcaSubtreeDistinctColors<Color, MAX_SIZE>(vertex_colors)`

頂点色を保持する。時間計算量は $O(n)$。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は償却 $O(1)$。

### `build(root)`

LCA・Euler順序・区間異色数索引を構築する。時間計算量は $O(n\log n)$。

### `count_at_lca_subtree(left, right)`

両頂点のLCAの部分木にある異なる色数を返す。時間計算量は $O(\log n)$。

## 注意点

- 辺集合は連結な木で、頂点数は `MAX_SIZE` 以下でなければならない。
- `build` 前のqueryや範囲外頂点では `runtime_error`。

## 容量を省略する可変長版

容量テンプレート引数をすべて省略すると、実行時サイズに合わせた `std::vector` を使う。容量を明示する固定長版も同じヘッダにある。

```cpp
LcaSubtreeDistinctColors<int> dynamic(colors);
LcaSubtreeDistinctColors<int, 100000> fixed(colors);
```

前処理・保存領域は $O(N\log N)$、クエリは $O(\log N)$ 時間。
