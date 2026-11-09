---
title: Left-Right Planarity Test (左右平面性判定)
documentation_of: ../../../../../src/algorithm/graph/specialized/structural_analysis/planarity_test.hpp
---

単純化した無向グラフが平面的かどうかを、Left-Right 平面性判定法で判定する。
DFS の後退辺に左右の制約を与え、両側へ同時に置けない衝突を検出する。
再帰を使わないため、長いパスでもコールスタックを消費しない。

## `is_planar_graph`

```cpp
bool is_planar_graph(
    int vertex_count,
    const vector<pair<int,int>>& edges
);
```

頂点集合を `[0, vertex_count)` とする無向グラフが平面的なら `true` を返す。
自己ループは平面性に影響しないため無視し、多重辺は 1 本の辺にまとめる。
頂点数 $N\ge 3$ で、まとめた後の辺数が $3N-6$ を超える場合は、探索を行わずに `false` を返す。

- 時間計算量: $O(N+M)$
- 空間計算量: $O(N+M)$

$N$ は頂点数、$M$ は辺数である。重複辺の除去と入れ子の深さ順の並べ替えには、8 bit を 1 桁とする 64 bit 整数の基数ソートを使う。各辺は定数回だけ走査する。

## 注意点

- 負の頂点数または範囲外の端点には `invalid_argument` を送出する。重複除去後の辺数が `int` に収まらない場合は `length_error` を送出する。
- この API は判定結果のみを返し、平面埋め込みや Kuratowski 部分グラフは構成しない。
