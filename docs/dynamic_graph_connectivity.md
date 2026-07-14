---
title: Dynamic Graph Connectivity (動的一般graph連結性) [DYNACON2]
documentation_of: ../src/structure/graph/dynamic_graph_connectivity.hpp
---

一般の無向graphについて辺追加・辺削除・連結性queryをonlineで扱う。

## API

### `DynamicGraphConnectivity<MAX_SIZE>(n)`

孤立頂点からなるgraphを作る。時間計算量は $O(n)$。

### `add_edge(left, right)`

辺を追加する。時間計算量は償却 $O(\log^2 n)$。

### `remove_edge(left, right)`

辺を1本削除する。時間計算量は償却 $O(\log^2 n)$。

### `connected(left, right)`

連結性を返す。時間計算量は償却 $O(\log n)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下でなければならない。
- self-loopは追加されない。多重辺は個数を管理し、削除は1本ずつ行う。
