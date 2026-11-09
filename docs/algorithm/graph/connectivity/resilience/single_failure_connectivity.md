---
title: Single Failure Connectivity Oracle (単一故障連結性オラクル)
documentation_of: ../../../../../src/algorithm/graph/connectivity/resilience/single_failure_connectivity.hpp
---

連結な無向グラフを前処理し、辺を 1 本、または頂点を 1 個だけ削除した後の 2 頂点の連結性に答える。
辺は入力順の 0-indexed の ID で指定する。多重辺と自己ループも扱える。

## クラス

```cpp
template<size_t MAX_VERTEX_COUNT = size_t(-1), size_t MAX_EDGE_COUNT = size_t(-1)>
class SingleFailureConnectivity;
```

最大頂点数と最大辺数をコンパイル時に指定する。作業領域はヒープに確保し、DFS は再帰を使わない。

テンプレート引数を両方とも指定すると固定容量の配列を使う。どちらかを省略すると（デフォルト値 `size_t(-1)`）、実行時のサイズに合わせた `std::vector` を使う可変長版になる。

```cpp
SingleFailureConnectivity<> dynamic(vertex_count, edges);
SingleFailureConnectivity<100000, 200000> fixed(vertex_count, edges);
```

## `SingleFailureConnectivity()`

未構築のオブジェクトを作る。

### 時間計算量

$O(1)$

## `SingleFailureConnectivity(vertex_count, edges)`

`vertex_count` 頂点のグラフを構築する。`edges[i]` は ID $i$ の辺の両端点である。

### 時間計算量

$O(N + M)$

## `build(vertex_count, edges)`

保持しているグラフを破棄し、コンストラクタと同じ形式のグラフで再構築する。

### 時間計算量

$O(N + M)$

## `vertex_count()`

構築済みグラフの頂点数を返す。未構築のときと、`build` が失敗した後は 0 を返す。

### 時間計算量

$O(1)$

## `edge_count()`

構築済みグラフの辺数を返す。未構築のときと、`build` が失敗した後は 0 を返す。

### 時間計算量

$O(1)$

## `connected_without_edge(first, second, edge_id)`

辺 `edge_id` だけを削除したグラフで `first` と `second` が連結なら `true` を返す。

### 時間計算量

$O(1)$

## `connected_without_vertex(first, second, removed)`

頂点 `removed` とそれに接続する辺を削除したグラフで `first` と `second` が連結なら `true` を返す。
`first == removed` または `second == removed` の場合は `false` を返す。

### 時間計算量

`removed` の DFS 木上の子の数を $d$ として $O(\log (d + 1))$。

## 空間計算量

固定容量版は作業領域として $O(\texttt{MAX_VERTEX_COUNT} + \texttt{MAX_EDGE_COUNT})$ を使う。可変長版は $O(N + M)$ を使う。

## 注意点

- 入力グラフは 1 頂点以上の連結な無向グラフでなければならない。
- 頂点は $[0, N)$、辺 ID は $[0, M)$ で指定する。
- 容量超過（頂点数 0 以下を含む）、範囲外の端点・頂点・辺 ID、非連結なグラフ、未構築状態でのクエリに対しては `std::runtime_error` を送出する。
- `build` が失敗した後は未構築状態になり、以前のグラフへのクエリはできない。
- 1 回のクエリで削除する辺または頂点は 1 個だけであり、グラフ自体は変更しない。
