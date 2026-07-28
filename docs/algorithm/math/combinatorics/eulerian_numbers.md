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

`order == 0` では `{1}`、それ以外では $k=0,\ldots,n-1$ の値を返す。

## 時間計算量

- `MOD` が `167772161`, `469762049`, `754974721`, `998244353` のいずれかで、
  必要なNTT長がその法で利用できる場合は $O(N\log N)$ 時間、$O(N)$ 空間。
  包除公式
  $\left\langle{N\atop k}\right\rangle=\sum_j(-1)^j\binom{N+1}{j}(k+1-j)^N$
  を1回の畳み込みで全 $k$ について評価する。それ以外は $O(N^2)$ 時間、$O(N)$ 空間。

## 注意点

結果は `MOD` で剰余を取る。法が素数でなくてもよく、その場合は除算を用いない
漸化式へ自動的に戻る。高速経路も必要NTT長が法の原始2冪根の上限を超える場合は
同じ漸化式を用いる。
