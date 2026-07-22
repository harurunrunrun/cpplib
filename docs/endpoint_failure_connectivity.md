---
title: Endpoint Failure Connectivity (端点指定単一故障連結性) [POLQUERY]
documentation_of: ../src/algorithm/graph/connectivity/endpoint_failure_connectivity.hpp
---

静的無向単純graphで、削除辺を両端から指定できる単一故障連結性oracleである。

## API

### `EndpointFailureConnectivity<MAX_VERTICES, MAX_EDGES>(vertex_count, edges)`

索引と辺端点辞書を構築する。時間計算量は $O(N+M)$。

### `connected_without_edge(first, second, edge_left, edge_right)`

指定辺を削除したときの連結性を返す。時間計算量は $O(1)$。

### `connected_without_vertex(first, second, removed)`

指定頂点を削除したときの連結性を返す。時間計算量は $O(1)$。

## 注意点

- 頂点数・辺数はtemplate上限以下でなければならない。
- self-loopと多重辺は扱わない。存在しない削除辺では `runtime_error`。
- 削除頂点がquery端点なら非連結を返す。
