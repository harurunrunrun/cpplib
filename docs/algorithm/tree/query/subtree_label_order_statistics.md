---
title: Subtree Label Order Statistics (部分木ラベル順序統計) [PT07J]
documentation_of: ../../../../src/algorithm/tree/query/subtree_label_order_statistics.hpp
---

静的な根付き木の各部分木について、（ラベル, 頂点番号）の辞書順で $k$ 番目の頂点を選ぶ。

## API

### `SubtreeLabelOrderStatistics<MAX_SIZE>(vertex_labels)`

```cpp
template<int MAX_SIZE = -1>
struct SubtreeLabelOrderStatistics;
explicit SubtreeLabelOrderStatistics(const std::vector<int>& vertex_labels);
```

各頂点のラベルを保持する。時間計算量は $O(n)$ である。
`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると固定容量の配列を使う。

```cpp
SubtreeLabelOrderStatistics<> dynamic(labels);
SubtreeLabelOrderStatistics<100000> fixed(labels);
```

### `size()`

頂点数を返す。時間計算量は $O(1)$ である。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は償却 $O(1)$ である。

### `build(root = 0)`

Euler 順序と順序統計の索引（Wavelet Matrix）を構築する。時間計算量は $O(n\log n)$、保持する領域は $O(n\log n)$ である。

### `kth_smallest_vertex(subtree_root, order)`

`subtree_root` の部分木内で、0-indexed で `order` 番目の頂点の番号を返す。時間計算量は $O(\log n)$ である。

## 注意点

- 木は連結でなければならない。固定容量版では頂点数が `MAX_SIZE` 以下でなければならない。
- 同じラベルの頂点どうしでは、頂点番号が小さいものを先に数える。
- `build` 前の呼び出しや、部分木のサイズ以上の `order` では `std::runtime_error` を送出する。
