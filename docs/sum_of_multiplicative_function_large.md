---
title: Sum of Multiplicative Function (Large) (大きなNまでの乗法的関数和)
documentation_of: ../src/algorithm/math/sum_of_multiplicative_function_large.hpp
---

素数冪での値と素数上の累積和が分かる乗法的関数を、
$N$ 以下で高速に総和する。現行 Library Checker の公式問題では

$$
f(p^e)=ae+bp,\qquad
\sum_{n=1}^{N}f(n)\pmod{469762049}
$$

を求め、$N\le 10^{13}$ である。乗法的関数なので $f(1)=1$ とする。
公式の最終出力はテストケースごとに1個の剰余である。

## 対応上限

```cpp
inline constexpr std::uint64_t
    math::sum_multiplicative_function_large_max_n;
```

保証する最大値 $10^{13}$ を表す。

## 素数累積和の2成分

```cpp
template<int MOD>
struct math::LargePrimePrefixComponents {
    std::vector<Modint<MOD>> prime_count;
    std::vector<Modint<MOD>> prime_sum;
};
```

同じ添字 `i` について次を保持する。

- `prime_count[i]`:
  $\pi(x_i)=\sum_{p\le x_i}1$
- `prime_sum[i]`:
  $\sum_{p\le x_i}p$

$x_i$ は `quotient_values()[i]` である。添字0は内部の番兵であり値0を持つ。
この2成分から、$f(p)=a+bp$ の素数累積和を
`a * prime_count[i] + b * prime_sum[i]` として作れる。

## 公式関数族の素数冪 evaluator

```cpp
template<int MOD>
struct math::LinearPrimePowerMultiplicativeFunction {
    Modint<MOD> exponent_coefficient;
    Modint<MOD> prime_coefficient;

    Modint<MOD> operator()(
        std::uint64_t prime,
        std::uint32_t exponent
    ) const;
};
```

`exponent_coefficient * exponent + prime_coefficient * prime`、
すなわち公式入力の $ae+bp$ を返す。`operator()` は $O(1)$。

## 汎用総和器

```cpp
template<int MOD>
class math::LargeMultiplicativeFunctionSummatory;

math::LargeMultiplicativeFunctionSummatory<469762049> summatory(n);
```

コンストラクタは $\sqrt n$ 以下の素数を列挙する。
`n > sum_multiplicative_function_large_max_n` では
`std::invalid_argument` を送出する。

### 上限を取得

```cpp
std::uint64_t limit() const;
```

構築時の `n` を返す。$O(1)$。

### 商テーブルの大きさ

```cpp
std::size_t quotient_table_size() const;
```

$\lfloor n/i\rfloor$ の相異なる値を格納する内部表の長さを返す。$O(1)$。

### 商テーブルの引数列

```cpp
std::vector<std::uint64_t> quotient_values() const;
```

各内部添字が表す $x_i$ を返す。添字0は0で、それ以外には
$\lfloor n/i\rfloor$ の相異なる値が入る。時間・追加領域は
$O(\sqrt n)$。

### 素数累積和の2成分を構築

```cpp
LargePrimePrefixComponents<MOD> prime_prefix_components() const;
```

`quotient_values()` と同じ配置で $\pi(x_i)$ と
$\sum_{p\le x_i}p$ を返す。素数版の商列篩を2成分同時に行う。

### 任意の乗法的関数を総和

```cpp
template<class PrimePowerFunction>
Modint<MOD> sum(
    PrimePowerFunction function,
    const std::vector<Modint<MOD>>& prime_prefix
) const;
```

`function(p, e)` が $f(p^e)$ を返す乗法的関数について
$\sum_{i=1}^{n}f(i)$ を返す。`prime_prefix[i]` には

$$
\sum_{p\le quotient\_values()[i]} f(p)
$$

を与える必要がある。表の長さが `quotient_table_size()` と異なる場合は
`std::invalid_argument` を送出する。

例として $f(n)=n^2$ では `function(p,e)=p^{2e}` とし、
各 $x_i$ までの素数平方和を `prime_prefix[i]` に入れる。
このように `quotient_values()` を使えば、$a+bp$ 以外の素数累積和 provider
も安全に接続できる。

## 公式問題用の高水準 API

```cpp
template<int MOD = 469762049>
Modint<MOD> math::sum_of_multiplicative_function_large(
    std::uint64_t n,
    Modint<MOD> exponent_coefficient,
    Modint<MOD> prime_coefficient
);
```

$f(p^e)=ae+bp$ を満たす乗法的関数の
$\sum_{i=1}^{n}f(i)$ を返す。内部で2成分の素数累積和を作って合成する。
`n == 0` では0を返し、保証上限超過では `std::invalid_argument` を送出する。

## アルゴリズム

$\lfloor N/i\rfloor$ の相異なる $O(\sqrt N)$ 個だけを保持し、
素数計数と素数和を Eratosthenes 型に更新する。その後、最小素因数と指数を固定する
DFS で素数冪を列挙する。$p^3$ が残り上限を超える枝は
素数累積和を使って一括処理するため、全整数や全合成数は列挙しない。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `sum_multiplicative_function_large_max_n` | $O(1)$ | $O(1)$ |
| `LinearPrimePowerMultiplicativeFunction::operator()` | $O(1)$ | $O(1)$ |
| constructor | $O(\sqrt n\log\log n)$ | オブジェクトに $O(\sqrt n/\log n)$ |
| `limit` / `quotient_table_size` | $O(1)$ | $O(1)$ |
| `quotient_values` | $O(\sqrt n)$ | $O(\sqrt n)$ |
| `prime_prefix_components` | $O(n^{3/4}/\log n)$ | $O(\sqrt n)$ |
| `sum` | $O(n^{3/4}/\log n)$ | 再帰スタック $O(\log n)$、入力表 $O(\sqrt n)$ |
| `sum_of_multiplicative_function_large` | $O(n^{3/4}/\log n)$ | $O(\sqrt n)$ |

## 注意点

- `MOD >= 1`。
- `function(p,e)` は `Modint<MOD>` に変換できる値を返す。
- `prime_prefix` は同じ `n` の総和器が返す商配置に対応させる。
- 公式入力では $1\le N\le10^{13}$、$0\le a,b<469762049$。
- 複数ケースでは公式 verifier が $T\sqrt N\le10^6$ も要求する。
