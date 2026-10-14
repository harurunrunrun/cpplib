---
title: Group Potential DSU (群ポテンシャル付き素集合) [unionfind_with_potential_non_commutative_group]
documentation_of: ../../../../src/structure/dsu/basic/group_potential_dsu.hpp
---

`GroupPotentialDSU<Group>(int n)` は非可換群の関係 `A[u] = A[v] * x` を管理する。
`Group` は `value_type`、`identity()`、`multiply(a,b)`、`inverse(a)` を提供する。

- `merge(u,v,x)`: 関係が整合すれば統合して `true`、矛盾すれば状態を変えず `false`。
- `difference(u,v)`: 同じ成分なら `A[v]^{-1}A[u]`、異なれば `nullopt`。
- `leader(v)`, `weight(v)`, `size()`。`weight(v)` は `A[v]=A[leader(v)]*weight(v)` の右因子。

`RollbackGroupPotentialDSU<Group>(n)` は同じ `merge`・`difference` を持ち、
`snapshot()` と `rollback(snapshot)` で過去の関係集合へ戻せる。
snapshot は成功した成分統合数であり、矛盾する関係や既に成立する関係では増加しない。

## 時間計算量

構築 $O(N)$、統合・照会は群演算を定数時間として償却 $O(\alpha(N))$、空間 $O(N)$。
rollback 版は統合・照会が $O(\log(N+1))$、snapshot は $O(1)$、rollback は取り消した統合数を $K$ として $O(K)$。

## 注意点

頂点は `[0,n)`。範囲外は `out_of_range`、負の頂点数は `invalid_argument`。
群演算の結合則・逆元・単位元および値の等値比較が必要。積の順序を交換してはならない。
