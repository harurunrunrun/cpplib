---
title: Floyd Warshall (ワーシャル・フロイド法) [GRL_1_C]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/all_pairs/floyd_warshall.hpp
---

全点対最短路を求める。負の辺を扱え、負閉路も検出する。

## 関数

```cpp
template <class T>
FloydWarshallResult<T> floyd_warshall(std::vector<std::vector<T>> dist, T inf)
```

`dist[i][j]` に辺 `i -> j` の重みを、辺がない要素に `inf` を入れて渡す。対角成分は正なら $0$ に置き換える。

戻り値 `FloydWarshallResult<T>` は次のメンバをもつ。

```cpp
std::vector<std::vector<T>> dist;
std::vector<std::vector<int>> next;
std::vector<std::vector<char>> reachable;
std::vector<char> negative;
bool has_negative_cycle;

std::vector<int> path(int from, int to) const;
```

- `dist[i][j]`: `i` から `j` への最短距離（`reachable[i][j]` が真のときだけ意味をもつ）。
- `next[i][j]`: 最短路で `i` の次に通る頂点。
- `reachable[i][j]`: `i` から `j` へ到達可能なら真。到達判定は `inf` と分離して管理しているため、間接経路の距離が `inf` 以上になっても正しく扱える。ただし入力行列では値が `inf` の要素を辺なしとみなすため、重みがちょうど `inf` の直接辺は表現できない。
- `negative[v]`: `v` が負閉路上にあれば真。
- `has_negative_cycle`: 負閉路が 1 つでもあれば真。
- `path(from, to)`: 最短路を両端を含む頂点列として復元する。到達不能な場合と、到達可能な負閉路を経由して最短距離が定まらない場合は空列を返す。

## 時間計算量

$N$ を頂点数、$L$ を `path` が返す頂点列の長さとする。

- `floyd_warshall`: $O(N^3)$
- `FloydWarshallResult::path(from, to)`: $O(N + L)$（負閉路の影響判定で全頂点を調べた後、高々 $N$ 頂点をたどる）

## 空間計算量

- `floyd_warshall` の戻り値: $O(N^2)$
- `path` の追加領域: 出力列の $O(L)$

## 注意点

- 入力は正方行列でなければならない。正方でない場合は `runtime_error` を送出する。
- `path` の引数が $[0, N)$ の範囲外なら `runtime_error` を送出する。
- 距離と辺の重みの加算結果は `T` で表現できなければならない。負閉路がある場合は距離が際限なく小さくなりうる点に注意する。
