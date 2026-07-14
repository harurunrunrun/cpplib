---
title: Dynamic Forest Connectivity (動的森連結性) [DYNACON1]
documentation_of: ../src/structure/tree/dynamic_forest_connectivity.hpp
---

常に森である無向graphの辺追加・削除・連結性を扱う。

## API

### `DynamicForestConnectivity<MAX_SIZE>(n)`

孤立頂点の森を作る。時間計算量は $O(n)$。

### `add_edge(left, right)`

非連結なら辺を追加して `true` を返す。時間計算量は償却 $O(\log n)$。

### `remove_edge(left, right)`

存在する木辺を削除できれば `true` を返す。時間計算量は償却 $O(\log n)$。

### `connected(left, right)`

連結性を返す。時間計算量は償却 $O(\log n)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下でなければならない。
- `add_edge` はcycleを作らず、`remove_edge` は指定端点間の直接辺だけを削除する。
