---
title: Quadratic Sieve Factorization (二次篩因数分解)
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/quadratic_sieve.hpp
---

$x^2 - n$ が因子基底（小さい素数の集合）上で滑らか（smooth）になる関係を集め、$\mathbb F_2$ 上の線形従属から $n$ の非自明な因数を得る二次篩法。

## `QuadraticSieveResult`

- `factor`（`std::optional<u64>`）: 見つかった因数。素数の入力では `factor == n`、偶数の入力では $2$ になる。
- `tested_values`: 試した $x$ の個数。
- `smooth_relations`: 集めた smooth な関係の個数。
- `factor_base_size`: 因子基底の大きさ。

## `quadratic_sieve_factor(value, factor_base_bound = 1000, maximum_tested_values = 1000000, extra_relations = 16)`

`factor_base_bound` 以下の素数を因子基底の候補とし、$\lceil \sqrt n \rceil$ から順に関係を集める（$n$ は `value`）。各素数 $p$ について $x^2 \equiv n \pmod p$ の根を Tonelli–Shanks 法で前計算し、根の合同類だけを 65536 要素ずつのブロックで正確に篩う。必要な関係の個数は因子基底の大きさに `extra_relations` を加えたものである。

## 時間計算量・空間計算量

因子基底の上限を $P$、因子基底の大きさを $B$、試した値の個数を $T$、smooth な関係の個数を $R$ とする。

- 時間計算量: 素数の列挙 $O(P \log \log P)$、根の前計算 $O(B \log^2 P)$、篩 $O(T \log \log P)$、smooth な関係の指数の復元 $O(RB)$、逐次的な $\mathbb F_2$ 上の消去 $O(RB^2/64)$。
- 空間計算量: $O(P + BR + B^2/64 + \min(T, 65536))$。

64 bit の剰余乗算には `unsigned __int128` を用いる。

## 注意点

- 試行回数などの資源に上限を設けた確率的な因数探索なので、上限内に十分な smooth な関係が得られない場合は `factor == std::nullopt` になる。
- `value < 2`、`factor_base_bound < 2`、`maximum_tested_values == 0` などの不正な引数には `std::invalid_argument` を送出する。
