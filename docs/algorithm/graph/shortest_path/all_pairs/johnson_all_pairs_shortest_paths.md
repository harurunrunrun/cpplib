---
title: Johnson All-Pairs Shortest Paths (Johnson法による全点対最短路)
documentation_of: ../../../../../src/algorithm/graph/shortest_path/all_pairs/johnson_all_pairs_shortest_paths.hpp
---

負の辺を含みうる疎な有向グラフの全点対最短路を Johnson 法で求める。グラフ全体に負閉路がない場合に使える。

## `JohnsonEdge<T>`

```cpp
template <class T>
struct JohnsonEdge { int from; int to; T cost; };
```

頂点 `from` から頂点 `to` への重み `cost` の有向辺を表す。多重辺と自己ループを許す。

構築と各メンバへのアクセスは $O(1)$ 時間、$O(1)$ 空間である。

## `johnson_all_pairs_shortest_paths`

```cpp
template <class T>
JohnsonAllPairsShortestPathsResult<T> johnson_all_pairs_shortest_paths(
    int vertex_count,
    const std::vector<JohnsonEdge<T>>& edges,
    T inf = std::numeric_limits<T>::max() / T(4)
);
```

全頂点へ重み $0$ の辺をもつ超始点を仮定した Bellman–Ford 法でポテンシャルを求め、重みを付け替えた非負辺のグラフ上で各頂点から Dijkstra 法を実行する。

`T` は `bool` 以外の符号付き整数型で、幅は 64 ビット以下とする。辺の重み、ポテンシャル、付け替え後の重み、探索中の距離は内部で `__int128_t` として計算する。有限な最短距離が `T` の範囲外になる場合は `overflow_error` を送出する。

戻り値 `JohnsonAllPairsShortestPathsResult<T>` は次のメンバをもつ。

```cpp
std::vector<std::vector<T>> dist;
std::vector<std::vector<int>> parent;
std::vector<std::vector<char>> reachable;
```

- `reachable[source][target]` は `source` から `target` へ到達可能なら真である。
- 到達可能なら `dist[source][target]` は有限な最短距離であり、`parent[source][target]` は選んだ最短路上で `target` の直前にある頂点である。`source == target` の `parent` は `-1` である。
- 到達不能なら `dist[source][target] == inf`、`parent[source][target] == -1` である。到達可能な距離が `inf` と等しくなることもあるため、到達判定には必ず `reachable` を使う。

Dijkstra 法のインデックス付きヒープは（付け替え後の距離, 頂点番号）の順に頂点を取り出し、`parent` は距離が真に短くなったときだけ更新する。この規則により、同じ入力グラフに対する `parent` と `path` は辺の入力順によらず決定的である。

## `JohnsonAllPairsShortestPathsResult::path`

```cpp
std::vector<int> path = result.path(from, to);
```

選ばれた `from` から `to` への最短路を、両端を含む頂点列として返す。到達不能なら空列を返す。

## 時間計算量

$N$ を頂点数、$M$ を辺数とする。Bellman–Ford 法は $O(NM)$、各始点からのインデックス付きヒープによる Dijkstra 法は $O((N + M) \log (N + 1))$、結果行列の構築は $O(N^2)$ である。全体は

$$
O\left(NM + N(N + M)\log(N + 1) + N^2\right)
$$

時間である。

`path` は返す頂点数を $L$ として $O(L)$ 時間である。

## 空間計算量

入力と戻り値を含めて $O(N + M + N^2)$。`path` は出力を含めて $O(L)$ の追加空間を使う。

## 注意点

- `vertex_count` は 0 以上、各辺の端点と `path` の引数は $[0, N)$ の範囲内でなければならない。違反時は `runtime_error` を送出する。
- グラフのどこかに負閉路がある場合は、最短距離表を返さずに `runtime_error` を送出する。特定の始点から到達不能な負閉路も検出の対象である。
- `inf` は到達不能な要素に格納する値にすぎず、計算上の上限ではない。
- `__int128_t` を提供する GCC 13 以降を対象とし、外部の多倍長整数ライブラリには依存しない。
