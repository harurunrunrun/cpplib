---
title: Root Path First Marked (根パス先頭マーク頂点) [QTREE3]
documentation_of: ../../../../src/structure/tree/query/root_path_first_marked.hpp
---

頂点のmarkを反転し、根から指定頂点へのパスで根に最も近いmark頂点を返す。

## API

### `RootPathFirstMarked<MAX_SIZE>(n)`

全頂点が未markの構造を作る。時間計算量は $O(n)$。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は償却 $O(1)$。

### `build(root = 0)`

根付きHL分解を構築する。時間計算量は $O(n)$。

### `toggle(vertex)`

markを反転し、反転後がmarkなら `true` を返す。時間計算量は $O(\log n)$。

### `first_marked(vertex)`

根からのパスで最初のmark頂点を返し、存在しなければ `-1`。時間計算量は $O(\log^2 n)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下で、辺集合は連結な木でなければならない。
- `build` 前の操作と範囲外頂点では `runtime_error`。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = RootPathFirstMarked<>;
using Fixed = RootPathFirstMarked<N>;
```

可変長版は実行時の要素数・頂点数に応じて配列を確保する。ノードを追加する構造では伸長費用が償却 $O(1)$、既存の操作計算量を維持する。
領域は使用ノード数に比例し、木の距離索引など既存の段別領域は上記と同じ漸近量。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
