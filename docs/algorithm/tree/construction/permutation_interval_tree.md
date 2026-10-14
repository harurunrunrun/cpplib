---
title: Permutation Strong Interval Tree (順列の強共通区間分解木) [common_interval_decomposition_tree]
documentation_of: ../../../../src/algorithm/tree/construction/permutation_interval_tree.hpp
---

`PermutationIntervalTree(permutation)` は `[0,N)` の順列の強共通区間分解を構築する。
`root()` は根の node 番号、空入力なら -1。`nodes()` は node 配列。
node は半開位置区間 `left,right`、半開値区間 `minimum,maximum`、`linear`、位置順の `children` を持つ。
`smallest_container(l,r)` は `[l,r)` を含む最小の強共通区間の node 番号を返す。

## 時間計算量

構築 $O(N\log(N+1))$、包含照会 $O(\log(N+1))$、保存空間 $O(N\log(N+1))$。
単調 stack と区間加算最大値の木で分解し、包含照会には二分累乗を使う。

## 注意点

順列以外は `invalid_argument`。包含照会は `0<=l<r<=N` で、違反時は `out_of_range`。
葉も `linear=true`。node 番号は構築順で、根が 0 とは限らない。
