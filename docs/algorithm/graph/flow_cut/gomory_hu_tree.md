---
title: Gomory--Hu Tree (Gomory--Hu木)
documentation_of: ../../../../src/algorithm/graph/flow_cut/gomory_hu_tree.hpp
---

非負容量の無向グラフについて、全頂点対の最小カットの値を重み付き木（Gomory–Hu 木）で表す。Gusfield のアルゴリズムで $N-1$ 回の最大流計算により構築する。

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
    const std::vector<GomoryHuEdge>& edges
)
```

Gomory–Hu 木を構築する。

```cpp
struct GomoryHuTree {
    std::vector<int> parent;
    std::vector<long long> cut_value;

    GomoryHuTree(std::vector<int> parent, std::vector<long long> cut_value);
    int size() const;
    std::vector<GomoryHuEdge> edges() const;
    long long min_cut_value(int left, int right) const;
};
```

- `parent[v]`: 木における親。根 `0` だけは `-1` である。
- `cut_value[v]`: `v` と `parent[v]` を結ぶ木辺の重み。
- `size()`: 頂点数を返す。
- `edges()`: 木辺を `(v, parent[v], cut_value[v])` の形で返す。
- `min_cut_value(left, right)`: 元のグラフにおける 2 頂点間の最小カットの値を返す（木上のパスの最小辺重み。ダブリングで求める）。

## 時間計算量

頂点数を $N$、辺数を $M$、Dinic 法 1 回の計算量を $F(N, M)$ とする。

- `gomory_hu_tree`: $O(N F(N, M) + N \log N)$
- `size`: $O(1)$
- `edges`: $O(N)$
- `min_cut_value`: $O(\log N)$

この実装で一般容量に対する Dinic 法の上界 $F(N, M) = O(N^2 M)$ を代入すると、構築は $O(N^3 M)$ である。

## 空間計算量

- 構築中: $O(N + M)$
- `GomoryHuTree`: $O(N \log N)$
- `edges` の戻り値: $O(N)$

## 注意点

- 容量は非負でなければならない。頂点の範囲や容量の条件に違反すると `runtime_error` を送出する。
- 自己ループを除く全辺の容量の和が `long long` に収まらない場合は `overflow_error` を送出する。
- 同じ頂点どうしの `min_cut_value` は `std::numeric_limits<long long>::max()` を返す。
- 非連結な 2 頂点の `min_cut_value` は $0$ である。
