---
title: Dirichlet Prefix Convolution and Inverse (Dirichlet積の累積和と逆元) [dirichlet_convolution_and_prefix_sums] [dirichlet_inverse_and_prefix_sums]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/dirichlet_prefix_convolution.hpp
---

正整数 $N$（引数 `maximum`）に対して、

$$
Q_N=\{\lfloor N/i\rfloor:1\le i\le N\}
$$

の要素を昇順に並べた点だけで、算術関数の累積和 $F(x) = \sum_{n \le x} f(n)$ を管理する。$Q_N$ の要素数は $O(\sqrt N)$ で、各 API の入力と出力は $x \in Q_N$ の昇順に並べた $F(x)$ の列である。

## ディリクレ積

```cpp
template<class T>
std::vector<T> math::dirichlet_prefix_convolution(
    std::uint64_t maximum,
    const std::vector<T>& first_prefix,
    const std::vector<T>& second_prefix
);
```

`first_prefix` と `second_prefix` がそれぞれ算術関数 $f, g$ の $Q_N$ 上の累積和を表すとき、ディリクレ積

$$
(f*g)(n)=\sum_{d\mid n}f(d)g(n/d)
$$

の累積和を同じ順序で返す。

## ディリクレ積による除算

```cpp
template<class T>
std::vector<T> math::dirichlet_prefix_division(
    std::uint64_t maximum,
    const std::vector<T>& dividend_prefix,
    const std::vector<T>& divisor_prefix
);
```

$h = f \ast g$ の累積和 `dividend_prefix` と $g$ の累積和 `divisor_prefix` から、$f$ の累積和を返す。`T{1} / divisor_prefix[0]`（すなわち $1/g(1)$）が定義されなければならない。

## ディリクレ逆元

```cpp
template<class T>
std::vector<T> math::dirichlet_prefix_inverse(
    std::uint64_t maximum,
    const std::vector<T>& prefix
);
```

$f(1) \ne 0$ である算術関数 $f$ に対し、$f \ast g = \delta_1$ を満たすディリクレ逆元 $g$ の累積和を返す（$\delta_1$ の累積和を `dirichlet_prefix_division` で $f$ により割る）。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（戻り値を含む） |
| --- | --- | --- |
| `dirichlet_prefix_convolution` | $O(N^{2/3})$ | $O(\sqrt N)$ |
| `dirichlet_prefix_division` | $O(N^{2/3})$ | $O(\sqrt N)$ |
| `dirichlet_prefix_inverse` | $O(N^{2/3})$ | $O(\sqrt N)$ |

## 注意点

- `maximum` は正でなければならない。
- 各入力の長さは $\lvert Q_N \rvert$ でなければならない。
- 条件に違反すると `std::invalid_argument` を送出する。
- `T` は加算・減算・乗算を持つ必要がある。除算と逆元ではさらに `T{1} / prefix[0]` が定義され、その値が数学的な逆元である必要がある。
