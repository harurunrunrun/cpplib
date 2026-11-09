---
title: Core Decomposition (コア分解)
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/core_decomposition.hpp
---

無向グラフの各頂点のコア数と退化順序を求める。

## API

```cpp
CoreDecompositionResult core_decomposition(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

`vertex_count` 頂点の無向グラフをコア分解する。`edges` の各要素は無向辺の両端を表す。多重辺は別々の辺として数え、自己ループはコア数に寄与しない。

```cpp
struct CoreDecompositionResult {
    vector<int> core_number;
    vector<int> degeneracy_order;
    int degeneracy;
};
```

- `core_number[v]`: 頂点 `v` を含む最大の $k$-コアの $k$
- `degeneracy_order`: 各時点で次数最小の頂点を取り除いていく順序
- `degeneracy`: グラフの退化数（`core_number` の最大値）

## 時間計算量

頂点数を $N$、自己ループを除く辺数を $M$ とする。

- 入力検査と隣接リストの構築: $O(N + M)$
- 全頂点のコア数と退化順序の計算（バケットソートによる）: $O(N + M)$
- `core_decomposition` 全体: $O(N + M)$

## 空間計算量

$O(N + M)$。

## 注意点

頂点番号は $[0, N)$ でなければならない。範囲外の頂点または負の頂点数を受け取ると `std::runtime_error` を送出する。頂点数 0 のグラフの退化数は 0 である。
