---
title: Eulerian Trail (オイラー路) [eulerian_trail_directed] [eulerian_trail_undirected]
documentation_of: ../../../../src/algorithm/graph/traversal/eulerian_trail.hpp
---

全辺をちょうど 1 回ずつ通るオイラー路を Hierholzer 法で求める。有向グラフ・無向グラフのどちらでも、多重辺と自己ループを扱える。

## 関数

```cpp
optional<EulerianTrailResult> directed_eulerian_trail(n, edges, start = -1)
optional<EulerianTrailResult> undirected_eulerian_trail(n, edges, start = -1)
```

`edges[i] = {u, v}` の `i` が辺の ID になる。有向版では $u\to v$ の有向辺、無向版では $u$ と $v$ を結ぶ無向辺と解釈する。

オイラー路が存在しなければ `nullopt` を返す。存在すれば次を返す。

```cpp
struct EulerianTrailResult {
    vector<int> vertices; // M + 1 vertices
    vector<int> edges;    // M edge ids
};
```

`edges[k]` は `vertices[k]` から `vertices[k+1]` へ進む辺の ID で、`edges` は全辺の ID の順列になる。

`start = -1` なら次数の条件から始点を自動で選ぶ。始点を指定した場合は、その頂点から始まるオイラー路が存在するときだけ返す。辺が 0 本の場合、始点を指定すればその頂点だけ、自動選択なら頂点 `0` だけからなる路を返し、$N=0$ なら両方の列が空になる。

## API別の時間計算量・空間計算量

$N$ を頂点数、$M$ を辺数とする。

- `directed_eulerian_trail`: $O(N+M)$
- `undirected_eulerian_trail`: $O(N+M)$
- 追加空間と返り値: $O(N+M)$

## 注意点

- 頂点数は非負、`start` は $-1$ または $[0,N)$、辺の端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。
- 記載した計算量には引数の検査と戻り値の構築を含む。
