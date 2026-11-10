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

各段で生成元の逆元も加えて重複と恒等置換を除き、軌道を BFS で求めて代表元（transversal）を作る。次段の生成元は Schreier 生成元 $t_{g(x)}^{-1} \, g \, t_x$ をすべて列挙し、重複を除いたものである。

段 $i$ の生成元数を $G_i$、軌道の大きさを $O_i$ とすると次のようになる。

- 時間計算量: $O\left(\sum_i n O_i G_i \log (O_i G_i)\right)$（Schreier 生成元の構成と、長さ $n$ の置換同士の比較によるソート）
- 追加空間計算量: $O(n \max_i O_i G_i)$

## 注意点

- `degree >= 0` が必要。各入力は $[0, \mathrm{degree})$ の全要素をちょうど 1 回ずつ含む必要がある。サイズ不一致、範囲外の値、像の重複では `runtime_error` を送出する。
- sifting（篩い分け）を行わない基本的な Schreier 法であり、次段の生成元数は最大で $G_{i+1} \le O_i G_i$ と段ごとに掛け算で増えうる。生成元が冗長な入力や $n$ が大きい入力では、中間の安定化群の生成元が急激に増えて非常に遅くなることがある。
