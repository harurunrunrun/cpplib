---
title: Permutation Group Order (置換群の位数)
documentation_of: ../../../../src/algorithm/math/combinatorics/permutation_group_order.hpp
---

生成元で与えた有限置換群の位数を、Schreier の補題に基づく安定化群列で求める。点 $0, 1, \ldots, n-1$ を順に固定していき、各段の軌道の大きさの積を返す。位数は $n!$ まで大きくなるため `BigInteger` を使う。

## 型

```cpp
struct PermutationGroupOrderResult{
    BigInteger order;
    std::vector<std::size_t> stabilizer_orbit_sizes;
};
```

`stabilizer_orbit_sizes[i]` は、点 $0, \ldots, i-1$ を固定する部分群による点 $i$ の軌道の大きさである。これらの積が `order` になる。

## `permutation_group_order`

```cpp
PermutationGroupOrderResult permutation_group_order(
    int degree,
    const std::vector<std::vector<int>>& generators
)
```

各置換は、`permutation[x]` を $x$ の像とする長さ `degree`（$= n$）の `std::vector` で与える。生成元が無い（または恒等置換のみの）場合は自明群で、位数は $1$ である。

基点を $0, 1, \ldots, n-1$ に固定した決定的な Schreier-Sims 法で計算する。段 $i$（基点 $0, \ldots, i-1$ を固定する強生成元だけを使う）の軌道と代表元（transversal）を BFS で作り、その段の Schreier 生成元 $t_{g(x)}^{-1} \, g \, t_x$ を上の段の代表元で篩い分ける（sifting）。篩い切れない元が出たら、その剰余を強生成元に加え、篩い分けが止まった段からやり直す。返す軌道の大きさは点ごとの安定化群列の不変量なので、生成元の与え方によらない。

入力の生成元数を $k$ とする。剰余を加えるたびに安定化群列のどこかが真に大きくなるので、追加される強生成元は $O(n \log n)$ 個（$\log_2 n!$ 以下）である。

- 時間計算量: $O(n^5 \log n \,(k + n \log n))$（やり直し 1 回あたり、各段で軌道 $O(n)$ × 強生成元 × 篩い分け $O(n^2)$）。実際にはこれよりずっと速いことが多い。
- 追加空間計算量: $O(n^3 + n(k + n \log n))$

## 注意点

- `degree >= 0` が必要。各入力は $[0, \mathrm{degree})$ の全要素をちょうど 1 回ずつ含む必要がある。サイズ不一致、範囲外の値、像の重複では `runtime_error` を送出する。
