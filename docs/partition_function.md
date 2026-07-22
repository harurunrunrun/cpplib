---
title: Partition Function (分割数) [partition_function]
documentation_of: ../src/algorithm/math/combinatorics/partition_function.hpp
---

## `partition_numbers`

```cpp
template<int MOD>
std::vector<Modint<MOD>> partition_numbers(std::size_t maximum_sum)
```

`result[n]` に、整数 $n$ の分割数 $p(n)$ を法 `MOD` で格納して返す。
返却列の長さは `maximum_sum + 1` で、$p(0)=1$。

Eulerの五角数定理

$$
\prod_{k\geq1}(1-x^k)=
1+\sum_{k\geq1}(-1)^k
\left(x^{k(3k-1)/2}+x^{k(3k+1)/2}\right)
$$

で疎な母関数の分母を作り、形式的冪級数逆元を計算する。

- 時間計算量: $O(M(N))$。この実装のNTTでは $O(N\log N)$
- 空間計算量（追加領域）: $O(N)$

ここで $N=\mathtt{maximum\_sum}+1$、$M(N)$ は長さ $N$ の多項式積の計算量。
必要な2冪NTT長が `MOD - 1` を割り切り、定数項1が逆元を持つ必要がある。
FPS実装が扱える長さを超える場合は `length_error` または `runtime_error` を投げる。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
