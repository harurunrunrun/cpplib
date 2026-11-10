---
title: Minimum Weighted Inversion Tree Order (重み付き転倒和最小の木の順序) [rooted_tree_topological_order_with_minimum_inversions]
documentation_of: ../../../../src/algorithm/tree/construction/minimum_inversion_tree_order.hpp
---

根付き木の頂点を、親が子より前に来るように並べる順列 $p$ のうち、重み付き転倒和 $\sum_{j \lt i} c_{p(i)} \, d_{p(j)}$（コードでは `sum_{j<i} c[order[i]] * d[order[j]]`）を最小化するものを求める。

## API

```cpp
struct MinimumInversionTreeOrderResult{
    std::vector<int> order;
    __int128 cost;
};
MinimumInversionTreeOrderResult minimum_inversion_tree_order(
    const std::vector<int>& parent, const std::vector<std::int64_t>& c,
    const std::vector<std::int64_t>& d)
MinimumInversionTreeOrderResult minimum_inversion_forest_order(
    const std::vector<int>& parent, const std::vector<std::int64_t>& c,
    const std::vector<std::int64_t>& d)
```

- `minimum_inversion_tree_order(parent, c, d)`: 根は `parent[root] == -1` を満たすただ 1 つの頂点である。`c`, `d` は非負の `std::int64_t` である。結果の `order` は頂点の順列、`cost` は最小値（GCC の `__int128`）である。
- `minimum_inversion_forest_order(parent, c, d)`: 根が複数ある森の前提関係も扱い、異なる木の頂点を自由に交互に並べた最適な順序を返す。内部では仮想的な根を追加して `minimum_inversion_tree_order` に帰着する。

優先度付きキューでブロックを統合し、密度が最適な成分を親側へ連結していく。

## 時間計算量

頂点数を $N$ として $O(N\log(N+1))$ 時間、$O(N)$ 空間である。

## 注意点

- `minimum_inversion_tree_order` の親配列は、ただ 1 つの根を持つ木でなければならない。
- 負の重み、配列の長さの不整合、根がない・複数ある・閉路があるなど木でない入力では `std::invalid_argument` を送出する。
- 親の値が範囲外の場合は `std::out_of_range` を送出する。
- 和、密度比較の交差積、総コストが `__int128` に収まる必要がある。
- 最小コストを達成する順序が複数あるときは、そのうちの 1 つを返す。空の入力も許す。
