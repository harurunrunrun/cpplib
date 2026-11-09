---
title: Enumerate Triangles (三角形列挙)
documentation_of: ../../../../../src/algorithm/graph/specialized/subgraph_analysis/enumerate_triangles.hpp
---

単純無向グラフの三角形を 1 回ずつ列挙する。各辺を次数の小さい頂点から大きい頂点へ（次数が等しければ番号の小さい方から）向け、長さ 2 の有向パスを調べる。

## 三角形の列挙

```cpp
template<class Callback>
long long enumerate_triangles(
    int vertex_count,
    const vector<pair<int, int>>& edges,
    Callback&& callback
)
```

三角形 $(a,b,c)$ ごとに `callback(a, b, c)` を 1 回呼ぶ。`callback` に渡す頂点は `a < b < c` を満たす。戻り値は三角形の個数である。

入力は自己ループと多重辺を含まない単純無向グラフでなければならない。頂点数が負、頂点番号が範囲外、または自己ループがある場合は `runtime_error` を送出する。多重辺は検査しない。

### 時間計算量

- $O(N+M\sqrt{M}+T_{\mathrm{cb}})$

$N$ は頂点数、$M$ は辺数、$T_{\mathrm{cb}}$ は `callback` の実行時間の合計である。

## 三角形の個数

```cpp
long long count_triangles(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

三角形の個数だけを返す。

### 時間計算量

- $O(N+M\sqrt{M})$

## 空間計算量

- $O(N+M)$

## 注意点

頂点番号は $[0,N)$ の 0-indexed で、`callback` に渡される頂点番号も同じである。
