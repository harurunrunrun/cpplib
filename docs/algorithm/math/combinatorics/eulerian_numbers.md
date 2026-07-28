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

$O(N^2)$ 時間、$O(N)$ 補助空間。戻り値を含めた空間も $O(N)$。

## 注意点

結果は `MOD` で剰余を取る。乗除算を必要としないため、法が素数である必要はない。
