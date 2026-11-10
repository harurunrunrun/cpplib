---
title: Eulerian Numbers (Eulerian数)
documentation_of: ../../../../src/algorithm/math/combinatorics/eulerian_numbers.hpp
---

$n$ 要素の順列のうち下降位置がちょうど $k$ 個であるものの数 $\left\langle{n\atop k}\right\rangle$ を列挙する。

## API

```cpp
template<int MOD>
vector<Modint<MOD>> math::eulerian_numbers(size_t order);
```

`order` を $N$ とする。$N = 0$ なら `{1}` を、それ以外なら $k = 0, \ldots, N-1$ に対する $\left\langle{N\atop k}\right\rangle$ を長さ $N$ の `std::vector` で返す。

## 時間計算量

- `MOD` が `167772161`, `469762049`, `754974721`, `998244353` のいずれかで、必要な NTT 長（$2N + 2$ 以上）がその法で利用できる場合は $O(N \log N)$ 時間、$O(N)$ 空間。包除公式

$$
\left\langle{N\atop k}\right\rangle=\sum_{j=0}^{k+1}(-1)^j\binom{N+1}{j}(k+1-j)^N
$$

  を 1 回の畳み込みで全 $k$ について評価する。
- それ以外は漸化式 $\left\langle{n\atop k}\right\rangle = (k+1)\left\langle{n-1\atop k}\right\rangle + (n-k)\left\langle{n-1\atop k-1}\right\rangle$ を用い、$O(N^2)$ 時間、$O(N)$ 空間。

## 注意点

結果は `MOD` で剰余を取った値である。法は素数でなくてもよく、上記 4 つ以外の法では除算を用いない漸化式を自動的に使う。上記の法でも、必要な NTT 長が法の 2 の冪の原始根の上限を超える場合は同じ漸化式を用いる。
