---
title: Single Failure Connectivity Oracle (単一故障連結性オラクル)
documentation_of: ../src/algorithm/graph/single_failure_connectivity.hpp
---

連結な無向グラフを前処理し、辺を1本または頂点を1個だけ削除した後の2頂点の連結性を答える。
辺は入力順の0始まりIDで指定する。平行辺と自己ループも扱える。

## クラス

```cpp
template<size_t MAX_VERTEX_COUNT, size_t MAX_EDGE_COUNT>
class SingleFailureConnectivity;
```

最大頂点数と最大辺数をコンパイル時に指定する。作業領域はヒープに確保され、DFSは再帰を使わない。

## `SingleFailureConnectivity()`

未構築のオブジェクトを作る。

### 時間計算量

$O(1)$

## `SingleFailureConnectivity(vertex_count, edges)`

`vertex_count` 頂点のグラフを構築する。`edges[i]` は辺ID $i$ の両端点である。

### 時間計算量

$O(N+M)$

## `build(vertex_count, edges)`

保持中のグラフを破棄し、コンストラクタと同じ形式のグラフを再構築する。

### 時間計算量

$O(N+M)$

## `vertex_count()`

構築済みグラフの頂点数を返す。未構築時と失敗した `build` の後は0を返す。

### 時間計算量

$O(1)$

## `edge_count()`

構築済みグラフの辺数を返す。未構築時と失敗した `build` の後は0を返す。

### 時間計算量

$O(1)$

## `connected_without_edge(first, second, edge_id)`

辺 `edge_id` だけを削除したグラフで、`first` と `second` が連結なら `true` を返す。

### 時間計算量

$O(1)$

## `connected_without_vertex(first, second, removed)`

頂点 `removed` とその接続辺を削除したグラフで、`first` と `second` が連結なら `true` を返す。
`first == removed` または `second == removed` の場合は `false` を返す。

### 時間計算量

`removed` のDFS木上の子の数を $d$ として $O(\log(d+1))$。

## 空間計算量

固定容量の作業領域として $O(\mathtt{MAX\_VERTEX\_COUNT}+\mathtt{MAX\_EDGE\_COUNT})$ を使う。
入力グラフの前処理済み部分は $O(N+M)$ である。

## 注意点

- 入力グラフは1頂点以上の連結な無向グラフでなければならない。
- 頂点は $[0,N)$、辺IDは $[0,M)$ で指定する。
- 容量超過、範囲外の端点、非連結グラフ、未構築状態での問い合わせでは `runtime_error` を送出する。
- 失敗した `build` の後は未構築状態になり、以前のグラフへの問い合わせはできない。
- 1回の問い合わせで削除する辺または頂点は1個だけであり、グラフ自体は変更しない。
