---
title: Monochromatic Component Size Tree (同色連結成分サイズ) [QTREE6]
documentation_of: ../../../../src/structure/tree/query/monochromatic_component_size_tree.hpp
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

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = MonochromaticComponentSizeTree<>;
using Fixed = MonochromaticComponentSizeTree<N>;
```

可変長版は実行時の要素数・頂点数に応じて配列を確保する。ノードを追加する構造では伸長費用が償却 $O(1)$、既存の操作計算量を維持する。
領域は使用ノード数に比例し、木の距離索引など既存の段別領域は上記と同じ漸近量。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
