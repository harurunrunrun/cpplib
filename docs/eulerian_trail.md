---
title: Eulerian Trail
documentation_of: ../src/algorithm/graph/eulerian_trail.hpp
---

全辺をちょうど1回ずつ通るEulerian trailをHierholzer法で求める。有向・無向の両方で多重辺と自己loopを扱える。

# 関数

```cpp
optional<EulerianTrailResult> directed_eulerian_trail(n, edges, start = -1)
optional<EulerianTrailResult> undirected_eulerian_trail(n, edges, start = -1)
```

`edges[i] = {u, v}` の `i` が辺idになる。有向版では $u\to v$、無向版では $u-v$ と解釈する。

Eulerian trailが存在しなければ `nullopt`。存在すれば次を返す。

```cpp
struct EulerianTrailResult {
    vector<int> vertices; // M + 1 vertices
    vector<int> edges;    // M edge ids
};
```

`edges[k]` は `vertices[k]` から `vertices[k+1]` へ進む辺idで、全辺idの置換になる。

`start = -1` なら次数条件から始点を自動選択する。始点を指定した場合、その頂点から始まるtrailが存在するときだけ返す。辺が0本なら、頂点があれば自動選択時は頂点0だけからなるtrailを返し、$N=0$ なら両方の列が空になる。

# 計算量

$N$ を頂点数、$M$ を辺数とする。

- `directed_eulerian_trail`: $O(N+M)$
- `undirected_eulerian_trail`: $O(N+M)$
- 追加空間と返り値: $O(N+M)$
