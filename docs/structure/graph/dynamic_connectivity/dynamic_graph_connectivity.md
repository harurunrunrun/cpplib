---
title: Dynamic Graph Connectivity (動的グラフ連結性) [DYNACON2]
documentation_of: ../../../../src/structure/graph/dynamic_connectivity/dynamic_graph_connectivity.hpp
---

一般無向グラフの辺追加・辺削除・連結性問い合わせをオンラインで扱う。

## API

### `DynamicGraphConnectivity<MAX_SIZE>(n)`

孤立頂点からなるグラフを作る。時間計算量は
$O(\log \mathtt{MAX\_SIZE})$。

### `add_edge(left, right)`

辺を1本追加する。新しい端点対なら `true`、既存端点対の多重度を
増やした場合は `false` を返す。

### `remove_edge(left, right)`

辺を1本削除する。削除できた場合は `true`、存在しない場合は
`false` を返す。

### `connected(left, right)`

2頂点が連結かを返す。

## 時間計算量・空間計算量

頂点数を $N$、異なる端点対の数を $M$ とする。

- `add_edge`, `remove_edge`: 償却 $O(\log^2 N+\log M)$
- `connected`: 償却 $O(\log N)$
- 保存領域: $O(N\log N+M)$

## 注意点

- 頂点数は `MAX_SIZE` 以下でなければならない。
- 自己ループは記録するが連結性には影響しない。
- 多重辺は個数を管理し、削除は1本ずつ行う。
