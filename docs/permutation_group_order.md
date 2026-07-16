---
title: Permutation Group Order (置換群の位数)
documentation_of: ../src/algorithm/math/permutation_group_order.hpp
---

生成元で与えた有限置換群の位数を、Schreierの補題による安定化群列で求める。
点 $0,1,\ldots,n-1$ を順に固定し、各段のorbit sizeの積を返す。位数は $n!$
まで増えるため `BigInteger` を使う。

## 型

```cpp
struct PermutationGroupOrderResult{
    BigInteger order;
    std::vector<std::size_t> stabilizer_orbit_sizes;
};
```

`stabilizer_orbit_sizes[i]` は点 $0,\ldots,i-1$ を固定する部分群による点 $i$
のorbit size。これらの積が `order`。

## `permutation_group_order`

```cpp
PermutationGroupOrderResult permutation_group_order(
    int degree,
    const std::vector<std::vector<int>>& generators
)
```

各置換は `permutation[x]` を $x$ の像とする長さ `degree` のvectorで与える。
生成元を持たない場合は自明群で位数1。

各段の生成元数を $G_i$、orbit sizeを $O_i$ とすると:

- 時間計算量: $O(\sum_i n O_i G_i + \sum_i O_iG_i\log(O_iG_i))$
- 追加空間計算量: $O(n\max_i(O_iG_i))$

## 注意点

`degree >= 0`。各入力は $[0,degree)$ の全要素を一度ずつ含む必要がある。
サイズ不一致、範囲外、重複像では `runtime_error`。基本Schreier--Simsであり、
生成元をランダムに圧縮する版ではないため、冗長な生成元が非常に多い入力では
中間の安定化群生成元が増えることがある。

