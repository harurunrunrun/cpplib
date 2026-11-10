---
title: Sum of Totient Function (オイラーのトーシェント関数和) [sum_of_totient_function]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/sum_of_totient_function.hpp
---

オイラーのトーシェント関数の総和

$$
\Phi(N)=\sum_{i=1}^{N}\varphi(i)
$$

を法 `MOD` で求める。

## 対応上限

```cpp
inline constexpr std::uint64_t math::sum_totient_function_max_n;
```

この実装が保証する $N$ の最大値 $10^{10}$ を表す定数。

## トーシェント関数の総和

```cpp
template<int MOD = 998244353>
Modint<MOD> math::sum_of_totient_function(std::uint64_t n);
```

$\sum_{i=1}^{n}\varphi(i)$ を `MOD` で割った余りを返す。`n == 0` では $0$ を返し、`n > sum_totient_function_max_n` では `std::invalid_argument` を送出する。

次の恒等式を使う。

$$
\Phi(x)=\frac{x(x+1)}2-
\sum_{l=2}^{x}
\left(\max\{r:\lfloor x/r\rfloor=\lfloor x/l\rfloor\}-l+1\right)
\Phi(\lfloor x/l\rfloor)
$$

$L=\lfloor N^{2/3}\rfloor$ 以下の値は線形篩ですべて前計算する。それより大きい引数は $\lfloor N/i \rfloor$ の相異なる値についてだけメモ化し、同じ商を持つ $l$ の区間を一度に処理する。

## 短い別名

```cpp
template<int MOD = 998244353>
Modint<MOD> math::sum_totient(std::uint64_t n);
```

`sum_of_totient_function<MOD>(n)` と同じ値を返す。例外の条件も同じである。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `sum_totient_function_max_n` | $O(1)$ | $O(1)$ |
| `sum_of_totient_function<MOD>(n)` | 期待 $O(n^{2/3})$ | $O(n^{2/3})$ |
| `sum_totient<MOD>(n)` | 期待 $O(n^{2/3})$ | $O(n^{2/3})$ |

メモ化に `std::unordered_map` を使うので、時間計算量は期待計算量である。三角数は $2$ で割ってから剰余をとるので、`MOD` を法とする $2$ の逆元は仮定しない。

## 注意点

- `MOD >= 1` が必要である（`static_assert` で検査する）。
- `n` は `sum_totient_function_max_n` 以下でなければならない。
- 戻り値は数学的な総和を `MOD` で割った余りである。
