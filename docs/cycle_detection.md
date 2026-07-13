---
title: Cycle Detection
documentation_of: ../src/algorithm/graph/cycle_detection.hpp
---

閉路検出。

# 関数

```cpp
directed_cycle(graph)
undirected_cycle(n, edges)
```

`directed_cycle` は有向グラフの閉路を1つ返す。

`undirected_cycle` は無向グラフの閉路を1つ返す。多重辺を扱える。

閉路がない場合は空配列を返す。

## 時間計算量

$L$ を返す閉路の頂点数とする。

- `directed_cycle`: $O(N+M)$、閉路列の構築 $O(L)$ を含む
- `undirected_cycle`: $O(N+M)$、無向隣接リストと閉路列の構築を含む

## 空間計算量

- `directed_cycle`: 戻り値を含めて $O(N)$
- `undirected_cycle`: 戻り値と無向隣接リストを含めて $O(N+M)$

## API契約・前提・例外

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
