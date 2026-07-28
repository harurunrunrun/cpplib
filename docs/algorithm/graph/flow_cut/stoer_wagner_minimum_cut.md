---
title: Stoer--Wagner Minimum Cut (Stoer--Wagner法による無向グラフ最小カット)
documentation_of: ../../../../src/algorithm/graph/flow_cut/stoer_wagner_minimum_cut.hpp
---

非負容量の無向グラフで、始点と終点を固定しない大域最小カットを求める。

## API

```cpp
struct StoerWagnerEdge {
    int from;
    int to;
    long long capacity;
};
```

無向辺とその非負容量を表す。多重辺を扱え、自己ループは無視する。

```cpp
StoerWagnerMinimumCutResult stoer_wagner_minimum_cut(
    int vertex_count,
    const vector<StoerWagnerEdge>& edges
)
```

```cpp
struct StoerWagnerMinimumCutResult {
    long long capacity;
    vector<int> side;
};
```

- `capacity`: 大域最小カットの容量
- `side`: 最小カットで分かれる一方の頂点集合。昇順

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- 入力検査と容量行列の構築: $O(N^2+M)$
- Stoer--Wagner法の全フェーズ: $O(N^3)$
- `side` の整列: $O(N\log(N+1))$
- 全体: $O(N^3+M)$

## 空間計算量

$O(N^2+M)$。

## 注意点

容量は非負でなければならない。頂点範囲または容量条件に違反すると `runtime_error` を送出する。全辺容量の和が `long long` を超える場合は `overflow_error` を送出する。頂点数が0または1なら容量 `0` を返す。
