---
title: Counting Square-free Integers (平方因子を持たない整数の数え上げ) [counting_squarefrees]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/counting_squarefrees.hpp
---

$1$ 以上 $N$ 以下の無平方数（どの素数の 2 乗でも割り切れない整数）の個数を求める。

## 対応上限

```cpp
inline constexpr std::uint64_t math::counting_squarefrees_max_n;
```

この実装が保証する $N$ の最大値 $10^{18}$ を表す定数。

## 無平方数を数える

```cpp
std::uint64_t math::count_squarefree_numbers(std::uint64_t n);
```

$1$ 以上 `n` 以下の無平方数の個数を返す。`n == 0` では $0$ を返し、`n > counting_squarefrees_max_n` では `std::invalid_argument` を送出する。

メビウス関数を $\mu$ とすると、答えは

$$
Q(N)=\sum_{d\le\sqrt N}\mu(d)\left\lfloor\frac{N}{d^2}\right\rfloor
$$

である。$I = \lfloor N^{1/5} \rfloor$、$D = \lfloor \sqrt{N/I} \rfloor$ として、$d \le D$ の項は篩で求めた $\mu$ を使って直接足す。残りの項は、メルテンス関数 $M(x) = \sum_{i \le x} \mu(i)$ の値 $M(\lfloor \sqrt{N/i} \rfloor)$（$1 \le i < I$）を、商が等しい区間をまとめる再帰式で求めて一括で加える。平方根と 5 乗根は浮動小数点で近似した後、整数演算で補正する。

## 短い別名

```cpp
std::uint64_t math::count_squarefree(std::uint64_t n);
```

`count_squarefree_numbers(n)` と同じ値を返す。例外の条件も同じである。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `counting_squarefrees_max_n` | $O(1)$ | $O(1)$ |
| `count_squarefree_numbers(n)` | $O(n^{2/5} \log \log n)$ | $O(n^{2/5})$ |
| `count_squarefree(n)` | $O(n^{2/5} \log \log n)$ | $O(n^{2/5})$ |

時間計算量の $\log \log n$ の因子はメビウス関数の篩による。メルテンス関数の部分は、商が等しい区間をまとめることで $O(n^{2/5})$ になる。

## 注意点

途中の和は `std::int64_t` で計算し、$N \le 10^{18}$ の範囲でオーバーフローしない。
