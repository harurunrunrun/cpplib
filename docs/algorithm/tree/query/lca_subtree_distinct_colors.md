---
title: LCA Subtree Distinct Colors (LCA 部分木色数) [ADAORANG]
documentation_of: ../../../../src/algorithm/tree/query/lca_subtree_distinct_colors.hpp
---

2 頂点の LCA を根とする部分木に現れる、相異なる色の個数を返す静的な構造である。

## API

### `LcaSubtreeDistinctColors<Color, MAX_SIZE>(vertex_colors)`

```cpp
template<class Color, int MAX_SIZE = -1>
struct LcaSubtreeDistinctColors;
explicit LcaSubtreeDistinctColors(const std::vector<Color>& vertex_colors);
```

各頂点の色を保持する。時間計算量は $O(n)$ である。
`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると固定容量の配列を使う。

```cpp
LcaSubtreeDistinctColors<int> dynamic(colors);
LcaSubtreeDistinctColors<int, 100000> fixed(colors);
```

### `add_edge(left, right)`

無向辺を追加する。時間計算量は償却 $O(1)$ である。

### `build(root)`

LCA、Euler 順序、区間の相異なる色数の索引（`StaticRangeCountDistinct`）を構築する。時間計算量は $O(n\log n)$、保持する領域は $O(n\log n)$ である。

### `count_at_lca_subtree(left, right)`

両頂点の LCA を根とする部分木にある相異なる色の個数を返す。時間計算量は $O(\log n)$ である。

## 注意点

- 辺集合は連結な木でなければならない。固定容量版では頂点数が `MAX_SIZE` 以下でなければならない。
- `build` 前のクエリや範囲外の頂点では `std::runtime_error` を送出する。
