---
title: Counting Square-free Integers (平方因子を持たない整数の数え上げ) [counting_squarefrees]
documentation_of: ../src/algorithm/math/number_theory/counting_squarefrees.hpp
---

$1$ 以上 $N$ 以下の無平方数（どの素数の2乗でも割り切れない整数）の個数を求める。

## 対応上限

```cpp
inline constexpr std::uint64_t math::counting_squarefrees_max_n;
```

この実装が保証する最大値 $10^{18}$ を表す定数。

## 無平方数を数える

```cpp
std::uint64_t math::count_squarefree_numbers(std::uint64_t n);
```

$1$ 以上 `n` 以下の無平方数の個数を返す。`n == 0` では0を返す。
`n > counting_squarefrees_max_n` では `std::invalid_argument` を送出する。

Möbius 関数を $\mu$ とすると答えは

$$
Q(N)=\sum_{d\le\sqrt N}\mu(d)\left\lfloor\frac{N}{d^2}\right\rfloor
$$

である。$I=\lfloor N^{1/5}\rfloor$、
$D=\lfloor\sqrt{N/I}\rfloor$ として $d\le D$ を篩で直接加算する。
残りは Mertens 関数 $M(x)=\sum_{i\le x}\mu(i)$ の
$M(\lfloor\sqrt{N/i}\rfloor)$ を $1\le i<I$ について商をまとめて求め、
同じ商を持つ区間を一括加算する。平方根と5乗根は整数演算で補正する。

## 短い別名

```cpp
std::uint64_t math::count_squarefree(std::uint64_t n);
```

`count_squarefree_numbers(n)` と同じ値を返す。例外条件も同じである。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `counting_squarefrees_max_n` | $O(1)$ | $O(1)$ |
| `count_squarefree_numbers(n)` | $O(n^{2/5}\log\log n)$ | $O(n^{2/5})$ |
| `count_squarefree(n)` | $O(n^{2/5}\log\log n)$ | $O(n^{2/5})$ |

時間計算量の対数因子は Möbius 篩による。Mertens 関数部分は
商区間をまとめることで $O(n^{2/5})$ になる。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
