---
title: Gomory--Hu Tree (Gomory--Hu木)
documentation_of: ../../../../src/algorithm/graph/flow_cut/gomory_hu_tree.hpp
---

非負容量の無向グラフの全頂点対最小カット値を、重み付き木で表す。

## API

```cpp
struct GomoryHuEdge {
    int from;
    int to;
    long long capacity;
};
```

無向辺とその非負容量を表す。多重辺を扱え、自己ループは最小カットに寄与しない。

```cpp
GomoryHuTree gomory_hu_tree(
    int vertex_count,
    const vector<GomoryHuEdge>& edges
)
```

Gomory--Hu木を構築する。

```cpp
struct GomoryHuTree {
    vector<int> parent;
    vector<long long> cut_value;

    int size() const;
    vector<GomoryHuEdge> edges() const;
    long long min_cut_value(int left, int right) const;
};
```

- `parent[v]`: 木での親。根 `0` だけは `-1`
- `cut_value[v]`: `v` と `parent[v]` を結ぶ木辺の重み
- `size()`: 頂点数
- `edges()`: 木辺を `(v, parent[v], cut_value[v])` として返す
- `min_cut_value(left, right)`: 元のグラフにおける2頂点間の最小カット値

## 時間計算量

頂点数を $N$、辺数を $M$、Dinic法1回の計算量を $F(N,M)$ とする。

- `gomory_hu_tree`: $O(NF(N,M)+N\log N)$
- `size`: $O(1)$
- `edges`: $O(N)$
- `min_cut_value`: $O(\log N)$

この実装で一般容量に対するDinic法の上界を代入すると、構築は $O(N^3M)$。

## 空間計算量

- 構築中: $O(N+M)$
- `GomoryHuTree`: $O(N\log N)$
- `edges` の戻り値: $O(N)$

## 注意点

容量は非負でなければならない。頂点範囲または容量条件に違反すると `runtime_error` を送出する。全辺容量の和が `long long` を超える場合は `overflow_error` を送出する。同じ頂点同士の `min_cut_value` は `numeric_limits<long long>::max()` を返す。
