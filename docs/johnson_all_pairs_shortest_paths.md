---
title: Johnson All-Pairs Shortest Paths (Johnson法による全点対最短路)
documentation_of: ../src/algorithm/graph/shortest_path/johnson_all_pairs_shortest_paths.hpp
---

負辺を許す疎な有向グラフの全点対最短路をJohnson法で求める。グラフ全体に負閉路がない場合に利用できる。

## `JohnsonEdge<T>`

```cpp
JohnsonEdge<T>{from, to, cost}
```

頂点 `from` から頂点 `to` への重み `cost` の有向辺を表す。平行辺と自己辺を許す。

構築と各memberへのアクセスは $O(1)$ 時間、$O(1)$ 空間である。

## `johnson_all_pairs_shortest_paths`

```cpp
auto result = johnson_all_pairs_shortest_paths<T>(
    vertex_count, edges, inf
);
```

全頂点へ重み0の辺を持つ超始点を仮定したBellman--Ford法でpotentialを求め、縮約後の非負辺グラフで各頂点からDijkstra法を実行する。

`T` は `bool` 以外の符号付き整数型で、幅は64 bit以下とする。辺重み、potential、縮約重み、探索中の距離は内部で `__int128_t` として計算する。有限な最短距離が `T` の範囲外になる場合は `overflow_error` を送出する。

戻り値 `JohnsonAllPairsShortestPathsResult<T>` は次のmemberを持つ。

```cpp
vector<vector<T>> dist;
vector<vector<int>> parent;
vector<vector<char>> reachable;
```

- `reachable[source][target]` は `source` から `target` へ到達可能なら真である。
- 到達可能なら `dist[source][target]` は有限な最短距離であり、`parent[source][target]` は選択した最短路上で `target` の直前にある頂点である。`source == target` のparentは `-1` である。
- 到達不能なら `dist[source][target] == inf`、`parent[source][target] == -1` である。到達可能な距離が `inf` と等しい場合もあるため、到達判定には必ず `reachable` を使う。

Dijkstra法のindexed heapは縮約距離、頂点番号の順に頂点を取り出し、parentは距離が真に短くなったときだけ更新する。この規則により、同じ入力グラフに対するparentとpathは辺の入力順によらず決定的である。

## 時間計算量

$N$ を頂点数、$M$ を辺数とする。Bellman--Ford法は $O(NM)$、各始点のindexed heap付きDijkstra法は $O((N+M)\log(N+1))$、結果行列の構築は $O(N^2)$ である。全体は

$$
O\left(NM+N(N+M)\log(N+1)+N^2\right)
$$

時間である。

## 空間計算量

入力と戻り値を含めて $O(N+M+N^2)$ 空間である。

## `JohnsonAllPairsShortestPathsResult::path`

```cpp
vector<int> path = result.path(from, to);
```

選択された `from` から `to` への最短路を、両端を含む頂点列として返す。到達不能なら空列を返す。

返す頂点数を $L$ とすると $O(L)$ 時間、出力を含めて $O(L)$ 追加空間である。

## 注意点

- `vertex_count` は0以上、各辺の端点と `path` の引数は頂点範囲内でなければならない。違反時は `runtime_error` を送出する。
- グラフのどこかに負閉路がある場合、最短距離表は返さず `runtime_error` を送出する。特定の始点から到達不能な負閉路も検出対象である。
- `inf` は到達不能要素へ格納する値にすぎず、計算上の上限ではない。
- `__int128_t` を提供するGCC 13以降を対象とし、外部の多倍長整数ライブラリには依存しない。
