---
title: Self-Adjusting Top Tree Internal (自己調整トップ木内部実装)
documentation_of: ../../../../src/structure/tree/dynamic_forest/top_tree_internal.hpp
---

`TopTree` と `LazyTopTree` が共有する、自己調整 Top Tree の本体である（`top_tree_internal::SelfAdjustingTopTree<Traits, MAX_SIZE>`）。

## 型

- `Type { Edge, Compress, Rake }`: クラスタの種類。
- `Vertex`: 頂点値、所属クラスタへのハンドル、頂点番号、表現木の根の印を持つ。
- `Cluster`: 正順・逆順の積、長さ、根の印を持つ。`toggle()` は向きを反転する。
- `Node`: クラスタの境界、子・親・rake 子、遅延作用、反転フラグを持つ内部ノード。
- `Statistics`: `edge`, `compress`, `rake`, `depth` を持ち、`total()` はクラスタの総数を返す。

`Traits` は `S`, `T`, `e`, `id`, `op`, `mapping`, `composition` を提供する。

## API

- `SelfAdjustingTopTree(int n = MAX_SIZE)`: 頂点数 `n` 用の領域を持つ空の内部 Top Tree を作る（可変長版では `n` のデフォルト値は `0`）。コピーは禁止で、ムーブに対応する。
- `create_vertex(value, index)`: 頂点と補助葉を作り、`Vertex*` を返す。
- `expose(vertex)`: 頂点を含むクラスタを expose して `Node*` を返す。
- `connected(left, right)`: 2 頂点が同じ表現木に属するかを返す。
- `link(left, right)` / `cut(left, right)`: 表現木を辺で結ぶ / 直接の辺を削除する。
- `path(left, right)`: 両端間の内部パスクラスタを返す。
- `set_vertex(vertex, value)` / `get_vertex(vertex)`: 頂点値を更新 / 取得する。
- `set_represented_root(vertex, value)`: 表現木の根の印を切り替える。
- `represented_root(vertex)`: 連結成分内で印が付いた最小の頂点番号を返す。
- `path_product(left, right)`: 端点を含む有向パスの積を返す。
- `path_apply(left, right, action)`: 端点を含むパスに作用させる。
- `statistics(vertex)`: 連結成分のクラスタ数と最大深さを返す。

## 時間計算量

$N$ を頂点数、$C$ を連結成分のクラスタ数とする。`create_vertex` は $O(1)$、`statistics` は $O(C)$ である。
その他の更新・パス操作はならし $O(\log N)$ である。

## 空間計算量（API 別の追加領域）

- コンストラクタ: 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は $O(n)$（内部頂点 $2n$ 個、クラスタノード $4n$ 個分を確保する）
- `create_vertex`、`expose`、`connected`、`link`、`cut`、`path`、`set_vertex`、`get_vertex`、`set_represented_root`、`represented_root`、`path_product`、`path_apply`: $O(1)$（生成するノードは確保済みの領域から割り当てる）
- `statistics`: 走査用スタックを含めて $O(C)$
- ムーブコンストラクタ / ムーブ代入: $O(1)$

## 注意点

- 公開されている `TopTree` / `LazyTopTree` の内部部品であり、通常は直接使用しない。
- `create_vertex` は 1 頂点につき内部頂点を 2 個使い、最大 `n`（固定長版は `MAX_SIZE`）頂点まで作れる。クラスタノードは最大 `4 * n` 個である。
- `link` は異なる木の頂点を、`cut` と `path` は存在する辺・連結な両端を要求する。これらは上位のラッパーが検査する。容量または内部不変条件の違反では `runtime_error` を送出する。
- `Vertex*` / `Node*` は、このオブジェクトのムーブまたは破棄の後には使用できない。
- 容量を指定した固定配列版と可変長版（`MAX_SIZE` 省略、デフォルト値 `-1`）は同じヘッダで利用できる。可変長版も構築時の `n` で領域を確保し、後から伸長はしない。
