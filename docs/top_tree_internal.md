---
title: Self-Adjusting Top Tree Internal (自己調整トップ木内部実装)
documentation_of: ../src/structure/tree/top_tree_internal.hpp
---

`TopTree` と `LazyTopTree` が共有する自己調整Top Tree本体。

## 型

- `Type { Edge, Compress, Rake }`：clusterの種類。
- `Vertex`：頂点値、所属clusterへのhandle、頂点番号、表現木の根markerを持つ。
- `Cluster`：正順・逆順の積、長さ、根markerを持つ。`toggle()` は向きを反転する。
- `Node`：cluster境界、子・親・rake子、遅延作用、反転flagを持つ内部node。
- `Statistics`：`edge`, `compress`, `rake`, `depth` を持ち、`total()` はcluster総数を返す。

`Traits` は `S`, `T`, `e`, `id`, `op`, `mapping`, `composition` を提供する。

## API

- `SelfAdjustingTopTree()`：空の内部Top Treeを作る。コピーは禁止、moveに対応する。
- `create_vertex(value,index)`：頂点と補助葉を作り、`Vertex*` を返す。
- `expose(vertex)`：頂点を含むclusterをexposeして `Node*` を返す。
- `connected(left,right)`：2頂点が同じ表現木に属するかを返す。
- `link(left,right)` / `cut(left,right)`：表現木を辺で結ぶ／直接辺を削除する。
- `path(left,right)`：両端間の内部path clusterを返す。
- `set_vertex(vertex,value)` / `get_vertex(vertex)`：頂点値を更新／取得する。
- `set_represented_root(vertex,value)`：表現木の根markerを切り替える。
- `represented_root(vertex)`：成分内で印が付いた最小頂点番号を返す。
- `path_product(left,right)`：端点を含む有向path積を返す。
- `path_apply(left,right,action)`：端点を含むpathへ作用する。
- `statistics(vertex)`：成分のcluster数と最大深さを返す。

## 時間計算量

`create_vertex` は $O(1)$、`statistics` は成分のcluster数を $C$ として $O(C)$。
その他の更新・path操作はならし $O(\log N)$、保存領域は
$O(\mathtt{MAX\_SIZE})$。

## 空間計算量（API別の追加領域）

- constructor、`create_vertex`、`expose`、`connected`、`link`、`cut`、`path`、`set_vertex`、`get_vertex`、`set_represented_root`、`represented_root`、`path_product`、`path_apply`: $O(1)$（生成nodeは固定arenaから確保）
- `statistics`: traversal stackを含め $O(C)$
- move constructor / move assignment: $O(1)$

本体の固定保存領域は $O(\mathtt{MAX\_SIZE})$ である。

## 注意点

- 公開 `TopTree` / `LazyTopTree` の内部部品であり、通常は直接使用しない。
- `create_vertex` は1頂点につき内部頂点2個を使用し、最大 `MAX_SIZE` 頂点。cluster nodeは最大 `4 * MAX_SIZE`。
- `link` は異なる木、`cut` と `path` は存在する辺・連結な両端を要求する。上位ラッパーがこれらを検査する。容量または内部不変条件の違反では `runtime_error`。
- `Vertex*` / `Node*` はこのオブジェクトのmoveまたは破棄後に使用できない。
