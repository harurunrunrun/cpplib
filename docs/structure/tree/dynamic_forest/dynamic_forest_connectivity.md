---
title: Dynamic Forest Connectivity (動的森連結性) [DYNACON1]
documentation_of: ../../../../src/structure/tree/dynamic_forest/dynamic_forest_connectivity.hpp
---

常に森である無向グラフについて、辺の追加・削除と連結判定を扱う。内部では `LinkCutTree` を使う。

## API

### `DynamicForestConnectivity<MAX_SIZE>(n)`

頂点数 `n` の孤立頂点からなる森を作る。`MAX_SIZE` は省略でき、デフォルト値 `-1` では可変長版になる。時間計算量は固定長版で $O(\texttt{MAX_SIZE})$、可変長版で $O(n)$ である。

### `add_edge(left, right)`

2 頂点が非連結なら辺を追加して `true` を、既に連結なら何もせず `false` を返す。時間計算量はならし $O(\log n)$ である。

### `remove_edge(left, right)`

2 頂点を直接結ぶ辺が存在すれば削除して `true` を、なければ `false` を返す。時間計算量はならし $O(\log n)$ である。

### `connected(left, right)`

2 頂点が連結かを返す。時間計算量はならし $O(\log n)$ である。

## 注意点

- 頂点数は `MAX_SIZE` 以下でなければならない。範囲外の頂点では `runtime_error` を送出する。
- `add_edge` は閉路を作らず、`remove_edge` は指定した端点間の直接の辺だけを削除する。
- 容量を指定した固定配列版（`DynamicForestConnectivity<N>`）と可変長版（`DynamicForestConnectivity<>`）は同じヘッダで利用できる。
