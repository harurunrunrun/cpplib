---
title: Partition Function (分割数) [partition_function]
documentation_of: ../../../../src/algorithm/math/combinatorics/partition_function.hpp
---

## `partition_numbers`

```cpp
template<int MOD>
std::vector<Modint<MOD>> partition_numbers(std::size_t maximum_sum)
```

`result[n]` に整数 $n$ の分割数 $p(n)$ を法 `MOD` で格納して返す。返り値の長さは `maximum_sum + 1` で、$p(0)=1$ である。

Euler の五角数定理

$$
\prod_{k\geq1}(1-x^k)=
1+\sum_{k\geq1}(-1)^k
\left(x^{k(3k-1)/2}+x^{k(3k+1)/2}\right)
$$

で分母の疎な冪級数を作り、その形式的冪級数としての逆元を計算する。

$N$ を `maximum_sum + 1`、$M(N)$ を長さ $N$ の多項式積の計算量とする。

- 時間計算量: $O(M(N))$。この実装の NTT では $O(N \log N)$
- 空間計算量（追加領域）: $O(N)$

## 注意点

- 必要な 2 の冪の NTT 長が `MOD - 1` を割り切る必要がある（`fps_inv` の制約）。
- `maximum_sum` が `int` の最大値以上なら `length_error` を送出する。FPS 実装が扱える長さを超える場合も `length_error` または `runtime_error` を送出する。
