---
title: Minimum Weighted Inversion Tree Order (重み付き転倒和最小の木の順序) [rooted_tree_topological_order_with_minimum_inversions]
documentation_of: ../../../../src/algorithm/tree/construction/minimum_inversion_tree_order.hpp
---

`minimum_inversion_tree_order(parent,c,d)` は親が子より前の順列を求め、
`sum_{j<i} c[order[i]]*d[order[j]]` を最小化する。
根は `parent[root]==-1` の任意の頂点。`c,d` は非負 `int64_t`。
結果の `order` は頂点順列、`cost` は GCC 13 の `__int128`。
優先度付き block 統合で最適な密度の成分を親側へ連結する。
`minimum_inversion_forest_order(parent,c,d)` は根が複数ある森の前提関係も扱い、
異なる木の頂点を自由に interleave した最適順序を返す。

## 時間計算量

頂点数 $N$ として $O(N\log(N+1))$ 時間、$O(N)$ 空間。

## 注意点

親配列はただ一つの根を持つ木。負の重み、長さ不整合、木以外は `invalid_argument`。
親範囲外は `out_of_range`。和・密度比較の交差積・総費用が `__int128` に収まること。
同じ最小費用の順序が複数あるときは一つを返す。空入力も許す。
