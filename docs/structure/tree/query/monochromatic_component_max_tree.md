---
title: Monochromatic Component Maximum Tree (同色連結成分最大値) [QTREE7]
documentation_of: ../../../../src/structure/tree/query/monochromatic_component_max_tree.hpp
---

2色の木で色反転・頂点値代入・同色連結成分最大値を扱う。

## API

### `MonochromaticComponentMaxTree<Value, MAX_SIZE>(n)`

空の構造を作る。時間計算量は $O(n)$。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は償却 $O(1)$。

### `set_color(vertex, color)` / `set_value(vertex, value)`

頂点の色または値を代入する。構築前は $O(1)$、構築後は償却 $O(\log n)$。

### `build()`

索引を構築する。時間計算量は $O(n\log n)$。

### `toggle(vertex)`

色を反転する。時間計算量は償却 $O(\log n)$。

### `component_maximum(vertex)`

同色連結成分の最大頂点値を返す。時間計算量は償却 $O(\log n)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下、辺集合は連結な木でなければならない。
- `Value` は `MaxMonoid` で比較できる必要がある。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = MonochromaticComponentMaxTree<Value>;
using Fixed = MonochromaticComponentMaxTree<Value, N>;
```

可変長版は実行時の要素数・頂点数に応じて配列を確保する。ノードを追加する構造では伸長費用が償却 $O(1)$、既存の操作計算量を維持する。
領域は使用ノード数に比例し、木の距離索引など既存の段別領域は上記と同じ漸近量。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
