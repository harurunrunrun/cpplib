---
title: Multipoint Evaluation
documentation_of: ../src/algorithm/math/multipoint_evaluation.hpp
---

積木と剰余木を使い、1個の多項式を多数の点でまとめて評価する。
評価点は重複していてもよい。

## API

```cpp
#include "src/algorithm/math/multipoint_evaluation.hpp"

template<int MOD>
std::vector<Modint<MOD>> math::multipoint_evaluation(
    math::Polynomial<MOD> polynomial,
    const std::vector<Modint<MOD>>& points
);
```

返り値の `i` 番目は `polynomial(points[i])` である。評価点が空なら空を返し、
0多項式は全ての点で0になる。

内部では葉に $(X-p_i)$ を置いた積木を作り、根から
`polynomial mod subtree_product` を伝播する。各葉に残る定数が評価値になる。

## 計算量

係数数を `n`、評価点数を `m`、長さ `k` の多項式積を
$M(k)=O(k\log k)$ とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `multipoint_evaluation` | $O(M(n+m)\log(m+1))$ | $O((n+m)\log(m+1))$ |

## 前提

- 剰余計算で現れる非零係数が `Modint<MOD>` 上で逆元を持つ必要がある。
- NTT に必要な2冪変換長が `MOD - 1` を割り切る必要がある。
