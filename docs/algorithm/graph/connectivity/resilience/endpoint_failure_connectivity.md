---
title: Endpoint Failure Connectivity (端点指定単一故障連結性) [POLQUERY]
documentation_of: ../../../../../src/algorithm/graph/connectivity/resilience/endpoint_failure_connectivity.hpp
---

静的な連結無向単純グラフについて、削除する辺を両端点で指定できる単一故障連結性オラクル。内部で `SingleFailureConnectivity` を使い、端点の組から辺 ID を引くハッシュ表（`std::unordered_map`）を併せて持つ。

## API

### `EndpointFailureConnectivity<MAX_VERTICES, MAX_EDGES>(vertex_count, edges)`

索引と、辺の端点から辺 ID を引く辞書を構築する。テンプレート引数は頂点数・辺数の上限であり、ともに省略すると（デフォルト値 $-1$）実行時のサイズに合わせた `std::vector` を使う可変長版になる。

```cpp
EndpointFailureConnectivity<> dynamic(vertex_count, edges);
EndpointFailureConnectivity<100000, 200000> fixed(vertex_count, edges);
```

時間計算量は期待 $O(N + M)$、空間計算量は $O(N + M)$。

### `connected_without_edge(first, second, edge_left, edge_right)`

端点 `edge_left`, `edge_right` の辺を削除したとき、`first` と `second` が連結かを返す。端点の順序は問わない。時間計算量は期待 $O(1)$。

### `connected_without_vertex(first, second, removed)`

頂点 `removed` を削除したとき、`first` と `second` が連結かを返す。削除頂点の DFS 木上の子の数を $d$ として、時間計算量は $O(\log (d + 1))$。

## 注意点

- 入力は 1 頂点以上の連結グラフでなければならず、頂点数・辺数はテンプレート引数の上限以下でなければならない（`SingleFailureConnectivity` の条件に従い、違反時は `std::runtime_error`）。
- 多重辺は扱わない。同じ端点の組を持つ辺が 2 本以上あるとコンストラクタが `std::runtime_error` を送出する。自己ループも想定しない。
- 存在しない辺を指定すると `std::runtime_error` を送出する。
- 削除頂点がクエリの端点であれば非連結（`false`）を返す。
