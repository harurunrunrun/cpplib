---
title: Segmented Sampled Prime Enumeration (区間篩による素数の間引き) [enumerate_primes]
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/enumerate_sampled_primes.hpp
---

`math::enumerate_sampled_primes(limit, stride=1, offset=0)` は `limit` 以下の素数の総数 `count` と、0始まりの素数番号が `offset mod stride` の素数列 `primes` を返す。`limit>=0`、`stride>0`、`0<=offset<stride`。

## 時間計算量

奇数のみの区間 Eratosthenes 篩を用いる。時間 $O(N\log\log(N+2))$、出力を除く領域 $O(\sqrt N)$。各基底素数について次に消す倍数を保持し、ブロックごとの切り上げ除算を避ける。

類題: 素数列を全保持せず、K番目ごとに取り出す。`stride=1` で通常の全列挙にもなる。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
