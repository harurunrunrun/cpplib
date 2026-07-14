---
title: Root Path First Marked (根パス先頭マーク頂点) [QTREE3]
documentation_of: ../src/structure/tree/root_path_first_marked.hpp
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
