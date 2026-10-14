---
title: Dynamic Forest Connectivity (動的森連結性) [DYNACON1]
documentation_of: ../../../../src/structure/tree/dynamic_forest/dynamic_forest_connectivity.hpp
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

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = DynamicForestConnectivity<>;
using Fixed = DynamicForestConnectivity<N>;
```

可変長版は実行時の要素数・頂点数に応じて配列を確保する。ノードを追加する構造では伸長費用が償却 $O(1)$、既存の操作計算量を維持する。
領域は使用ノード数に比例し、木の距離索引など既存の段別領域は上記と同じ漸近量。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
