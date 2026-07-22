---
title: Left-Right Planarity Test (左右平面性判定)
documentation_of: ../src/algorithm/graph/specialized/planarity_test.hpp
---

単純化した無向グラフが平面的かをLeft-Right planarity testで判定する。
DFSのreturn edgeに左右制約を与え、両側へ同時に置けない衝突を検出する。
再帰を使わないため、長いpathでもcall stackを消費しない。

## `is_planar_graph`

```cpp
bool is_planar_graph(
    int vertex_count,
    const vector<pair<int,int>>& edges
);
```

頂点集合を `[0, vertex_count)` とする無向グラフが平面的なら `true` を返す。
self-loopは平面性を変えないため無視し、平行辺は一つの辺へまとめる。

- 時間計算量: $O((N+M)\log M)$
- 空間計算量: $O(N+M)$

DFSと制約処理自体は線形である。上記の対数因子は重複辺の正規化とnesting depth順の
比較sortによる。

## 注意点

負の頂点数または範囲外端点には `invalid_argument` を送出する。
このAPIは判定のみを返し、平面埋め込みやKuratowski subgraphは構成しない。
