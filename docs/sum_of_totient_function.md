---
title: Sum of Totient Function (オイラーのトーシェント関数和) [sum_of_totient_function]
documentation_of: ../src/algorithm/math/number_theory/sum_of_totient_function.hpp
---

Euler のトーティエント関数の総和

$$
\Phi(N)=\sum_{i=1}^{N}\varphi(i)
$$

を法 `MOD` で求める。

## 対応上限

```cpp
inline constexpr std::uint64_t math::sum_totient_function_max_n;
```

この実装が保証する最大値 $10^{10}$ を表す定数。

## トーティエント関数の総和

```cpp
template<int MOD = 998244353>
Modint<MOD> math::sum_of_totient_function(std::uint64_t n);
```

$\sum_{i=1}^{n}\varphi(i)\bmod MOD$ を返す。`n == 0` では0を返す。
`n > sum_totient_function_max_n` では `std::invalid_argument` を送出する。

次の恒等式を使う。

$$
\Phi(x)=\frac{x(x+1)}2-
\sum_{l=2}^{x}
\left(\max\{r:\lfloor x/r\rfloor=\lfloor x/l\rfloor\}-l+1\right)
\Phi(\lfloor x/l\rfloor)
$$

$L=\lfloor N^{2/3}\rfloor$ 以下は Euler の線形篩で全て前計算する。
それより大きい引数は、`floor(N / i)` の異なる値だけをメモ化し、
同じ商を持つ `l` の区間を一度に処理する。

## 短い別名

```cpp
template<int MOD = 998244353>
Modint<MOD> math::sum_totient(std::uint64_t n);
```

`sum_of_totient_function<MOD>(n)` と同じ値を返す。例外条件も同じである。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `sum_totient_function_max_n` | $O(1)$ | $O(1)$ |
| `sum_of_totient_function<MOD>(n)` | 期待 $O(n^{2/3})$ | $O(n^{2/3})$ |
| `sum_totient<MOD>(n)` | 期待 $O(n^{2/3})$ | $O(n^{2/3})$ |

メモ化に `std::unordered_map` を使うため、時間計算量は期待計算量である。
`MOD >= 1` が必要であり、三角数は2で割ってから剰余を取るので
2の逆元を仮定しない。

## 注意点

- `MOD >= 1` が必要である。
- `n` は `sum_totient_function_max_n` 以下でなければならない。
- 返り値は数学的な総和を `MOD` で割った剰余である。
- 内部のメモ化にhash tableを使うため、計算量は期待計算量である。
