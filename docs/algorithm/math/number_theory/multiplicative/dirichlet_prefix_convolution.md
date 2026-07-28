---
title: Dirichlet Prefix Convolution and Inverse (Dirichlet積の累積和と逆元) [dirichlet_convolution_and_prefix_sums] [dirichlet_inverse_and_prefix_sums]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/dirichlet_prefix_convolution.hpp
---

正整数 `maximum` に対し、

$$
Q_N=\{\lfloor N/i\rfloor:1\le i\le N\}
$$

を昇順に並べた点だけで算術関数の累積和を管理する。
長さは $O(\sqrt N)$ であり、各 API の入力と出力はこの順で並べる。

## Dirichlet積

```cpp
template<class T>
vector<T> math::dirichlet_prefix_convolution(
    uint64_t maximum,
    const vector<T>& first_prefix,
    const vector<T>& second_prefix
);
```

`first_prefix` と `second_prefix` がそれぞれ算術関数 $f,g$ の
$Q_N$ 上の累積和を表すとき、Dirichlet積

$$
(f*g)(n)=\sum_{d\mid n}f(d)g(n/d)
$$

の累積和を同じ順序で返す。

## Dirichlet積による除算

```cpp
template<class T>
vector<T> math::dirichlet_prefix_division(
    uint64_t maximum,
    const vector<T>& dividend_prefix,
    const vector<T>& divisor_prefix
);
```

$h=f*g$ の累積和 `dividend_prefix` と $g$ の累積和
`divisor_prefix` から、$f$ の累積和を返す。
`T{1} / divisor_prefix[0]` が定義されなければならない。

## Dirichlet逆元

```cpp
template<class T>
vector<T> math::dirichlet_prefix_inverse(
    uint64_t maximum,
    const vector<T>& prefix
);
```

$f(1)\ne0$ である算術関数 $f$ に対し、
$f*g=\delta_1$ を満たす Dirichlet 逆元 $g$ の累積和を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（戻り値を含む） |
| --- | --- | --- |
| `dirichlet_prefix_convolution` | $O(N^{2/3})$ | $O(\sqrt N)$ |
| `dirichlet_prefix_division` | $O(N^{2/3})$ | $O(\sqrt N)$ |
| `dirichlet_prefix_inverse` | $O(N^{2/3})$ | $O(\sqrt N)$ |

## 注意点

- `maximum` は正でなければならない。
- 各表の長さは $|Q_N|$ でなければならない。
- 条件違反では `std::invalid_argument` を送出する。
- `T` は加減乗算を持つ必要がある。除算と逆元ではさらに
  `T{1} / prefix[0]` が定義され、その値が数学的な逆元である必要がある。
