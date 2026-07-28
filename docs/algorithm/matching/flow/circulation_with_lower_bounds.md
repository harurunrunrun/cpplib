---
title: Circulation with Lower Bounds (下限制約付き循環流)
documentation_of: ../../../../src/algorithm/matching/flow/circulation_with_lower_bounds.hpp
---

各有向辺の流量下限・上限と全頂点の流量保存則を同時に満たす循環流を求める。

## API

```cpp
struct LowerBoundCirculationEdge {
    int from;
    int to;
    long long lower;
    long long upper;
};
```

```cpp
optional<LowerBoundCirculationResult> circulation_with_lower_bounds(
    int vertex_count,
    const vector<LowerBoundCirculationEdge>& edges
)
```

実行可能な循環流が存在しなければ `nullopt` を返す。

```cpp
struct LowerBoundCirculationResult {
    vector<long long> flow;
};
```

`flow[i]` は入力辺 `i` の流量で、`lower <= flow[i] <= upper` を満たす。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- 下限を除去した補助ネットワークの構築: $O(N+M)$
- Dinic法による実行可能性判定: 一般容量で $O(N^2M)$
- 元の辺流量の復元: $O(M)$

## 空間計算量

$O(N+M)$。

## 注意点

`0 <= lower <= upper` かつ辺端点が $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。需要または頂点収支が `long long` を超える場合は `overflow_error` を送出する。多重辺と自己ループを扱える。
