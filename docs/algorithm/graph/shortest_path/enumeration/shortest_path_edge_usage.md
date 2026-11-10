---
title: Shortest Walk Edge Usage (最短歩道での辺利用判定)
documentation_of: ../../../../../src/algorithm/graph/shortest_path/enumeration/shortest_path_edge_usage.hpp
---

各辺が `source` から `target` への最小コストの歩道（walk）のいずれかに含まれるかを判定する。歩道では頂点と辺の再訪を許す。そのため重み $0$ の閉路が最小コストの歩道上にある場合、最短の単純パスには含まれない閉路上の辺も `used` になりうる。

辺の重みは非負でなければならない。

## 関数

```cpp
template <class T>
ShortestPathEdgeUsageResult<T> shortest_path_edge_usage(
    int n,
    const std::vector<ShortestPathEdgeUsageEdge<T>>& edges,
    int source,
    int target,
    T inf = std::numeric_limits<T>::max() / 4
)
```

`edges` は `ShortestPathEdgeUsageEdge<T>{from, to, cost}` の列で、結果は入力順の添字で対応づける。

戻り値 `ShortestPathEdgeUsageResult<T>` は次のメンバをもつ。

```cpp
T shortest;
bool reachable;
std::vector<char> used;
std::vector<char> unused;
```

- `used[i]`: 辺 `i` が少なくとも 1 つの最小コストの歩道に含まれるなら真。判定には $\mathrm{dist}(\mathrm{source}, \mathrm{from}) + \mathrm{cost} + \mathrm{dist}(\mathrm{to}, \mathrm{target}) = \mathrm{shortest}$ を用いる。
- `unused[i]`: `used[i]` の否定。
- `reachable`: `target` に到達可能なら真。到達不能ならすべての辺が `unused` になる。
- `shortest`: 最短距離。到達不能なら `inf`。

`inf` は到達不能なときの `shortest` に格納する初期値にすぎず、到達可能な最短距離が `inf` 以上でも正しく扱える。

## 時間計算量

$N$ を頂点数、$M$ を辺数として $O((N + M)\log(N + M + 1))$。

順方向・逆方向の Dijkstra 法を 1 回ずつ行い、最後に $M$ 本の辺について `used`, `unused` を構築する。

## 空間計算量

戻り値と 2 つの隣接リストを含めて $O(N + M)$。

## 注意点

- `n` が負、`source`, `target` や辺の端点が $[0, n)$ の範囲外、または負の重みの辺がある場合は `runtime_error` を送出する。
- 距離と辺の重みの加算結果は `T` で表現できなければならない。浮動小数点型では等号判定が丸め誤差の影響を受ける。
