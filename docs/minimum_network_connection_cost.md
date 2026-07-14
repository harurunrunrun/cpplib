---
title: Minimum Network Connection Cost (最小ネットワーク接続費) [BLINNET]
documentation_of: ../src/algorithm/graph/minimum_network_connection_cost.hpp
---

# MinimumNetworkConnectionEdge

```cpp
struct MinimumNetworkConnectionEdge {
    int from;
    int to;
    long long cost;
};
```

接続候補の両端と費用を表す。

# minimum_network_connection_cost

```cpp
long long minimum_network_connection_cost(
    int vertex_count,
    const vector<MinimumNetworkConnectionEdge>& edges
)
```

全頂点を接続するために必要な費用の最小値を返す。

## 引数

- `vertex_count`: 頂点数。
- `edges`: 無向の接続候補。

## 戻り値

最小接続費用。

## API別の時間計算量・空間計算量

$N=vertex_count$, $M=edges.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_network_connection_cost` | $O(N+M\log(M+1))$ | $O(N+M)$ |

## 注意点

- `vertex_count` は非負、端点は $[0,N)$、費用は非負でなければならない。
- グラフが非連結なら `invalid_argument`、答えが `long long` を越える場合は `overflow_error` を送出する。
