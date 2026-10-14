---
title: Endpoint Failure Connectivity (端点指定単一故障連結性) [POLQUERY]
documentation_of: ../../../../../src/algorithm/graph/connectivity/resilience/endpoint_failure_connectivity.hpp
---

静的無向単純graphで、削除辺を両端から指定できる単一故障連結性oracleである。

## API

### `EndpointFailureConnectivity<MAX_VERTICES, MAX_EDGES>(vertex_count, edges)`

索引と辺端点辞書を構築する。時間計算量は $O(N+M)$。

### `connected_without_edge(first, second, edge_left, edge_right)`

指定辺を削除したときの連結性を返す。時間計算量は $O(1)$。

### `connected_without_vertex(first, second, removed)`

指定頂点を削除したときの連結性を返す。削除頂点の DFS 木上の子数を $d$ として、
時間計算量は $O(\log(d+1))$。

## 注意点

- 頂点数・辺数はtemplate上限以下でなければならない。
- self-loopと多重辺は扱わない。存在しない削除辺では `runtime_error`。
- 削除頂点がquery端点なら非連結を返す。

## 容量を省略する可変長版

容量テンプレート引数をすべて省略すると、実行時サイズに合わせた `std::vector` を使う。容量を明示する固定長版も同じヘッダにある。

```cpp
EndpointFailureConnectivity<> dynamic(vertex_count, edges);
EndpointFailureConnectivity<100000, 200000> fixed(vertex_count, edges);
```

構築は期待 $O(N+E)$ 時間・$O(N+E)$ 空間、端点指定の辺検索は期待 $O(1)$ 時間。
頂点削除判定は、削除頂点の DFS 木上の子数を $d$ として $O(\log(d+1))$ 時間。
