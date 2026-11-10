---
title: Sum of Multiplicative Function (Large) (大きなNまでの乗法的関数和) [sum_of_multiplicative_function] [sum_of_multiplicative_function_large]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/sum_of_multiplicative_function_large.hpp
---

素数冪での値と素数上の累積和が分かる乗法的関数 $f$ について、$N$ 以下での総和を高速に求める。乗法的関数なので $f(1)=1$ とする。付属の `LinearPrimePowerMultiplicativeFunction` は、次の関数族を表す。

$$
f(p^e)=ae+bp
$$

## 対応上限

```cpp
inline constexpr std::uint64_t
    math::sum_multiplicative_function_large_max_n;
```

この実装が保証する $N$ の最大値 $10^{13}$ を表す定数。

## 素数累積和の 2 成分

```cpp
template<int MOD>
struct math::LargePrimePrefixComponents {
    std::vector<Modint<MOD>> prime_count;
    std::vector<Modint<MOD>> prime_sum;
};
```

同じ添字 `i` について次を保持する。

- `prime_count[i]`: $\pi(x_i)=\sum_{p\le x_i}1$
- `prime_sum[i]`: $\sum_{p\le x_i}p$

$x_i$ は `quotient_values()[i]` である。添字 $0$ は内部の番兵で、値 $0$ を持つ。この 2 成分から、$f(p)=a+bp$ の素数累積和を `a * prime_count[i] + b * prime_sum[i]` として作れる。

## 付属の関数族の素数冪での値

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

`exponent_coefficient * exponent + prime_coefficient * prime`、すなわち $ae+bp$ を返す。`operator()` は $O(1)$。

## 汎用の総和クラス

```cpp
template<int MOD>
class math::LargeMultiplicativeFunctionSummatory;

math::LargeMultiplicativeFunctionSummatory<469762049> summatory(n);
```

コンストラクタは $\sqrt n$ 以下の素数を列挙する。`n > sum_multiplicative_function_large_max_n` では `std::invalid_argument` を送出する。

### 上限の取得

```cpp
std::uint64_t limit() const;
```

構築時の `n` を返す。$O(1)$。

### 商の表の大きさ

```cpp
std::size_t quotient_table_size() const;
```

$\lfloor n/i\rfloor$ の相異なる値を格納する内部表の長さ（番兵を含む）を返す。$O(1)$。

### 商の表の引数列

```cpp
std::vector<std::uint64_t> quotient_values() const;
```

各内部添字が表す $x_i$ を返す。添字 $0$ は $0$ で、それ以外には $\lfloor n/i\rfloor$ の相異なる値が降順に入る。時間・追加領域は $O(\sqrt n)$。

### 素数累積和の 2 成分の構築

```cpp
LargePrimePrefixComponents<MOD> prime_prefix_components() const;
```

`quotient_values()` と同じ配置で $\pi(x_i)$ と $\sum_{p\le x_i}p$ を返す。素数についての商ごとの篩を 2 成分同時に行う。

### 任意の乗法的関数の総和

```cpp
template<class PrimePowerFunction>
Modint<MOD> sum(
    PrimePowerFunction function,
    const std::vector<Modint<MOD>>& prime_prefix
) const;
```

`function(p, e)` が $f(p^e)$ を返す乗法的関数について $\sum_{i=1}^{n}f(i)$ を返す。`prime_prefix[i]` には、$x_i$ を `quotient_values()[i]` として

$$
\sum_{p\le x_i} f(p)
$$

を与える必要がある。表の長さが `quotient_table_size()` と異なる場合（$n = 0$ では表が空でない場合）は `std::invalid_argument` を送出する。

例として $f(n)=n^2$ では `function(p, e)` が $p^{2e}$ を返すようにし、各 $x_i$ までの素数の平方和を `prime_prefix[i]` に入れる。このように `quotient_values()` を使えば、$a+bp$ 以外の形の素数累積和を求める処理とも安全に組み合わせられる。

## 付属の関数族用の API

```cpp
template<int MOD = 469762049>
Modint<MOD> math::sum_of_multiplicative_function_large(
    std::uint64_t n,
    Modint<MOD> exponent_coefficient,
    Modint<MOD> prime_coefficient
);
```

$f(p^e)=ae+bp$（$a$ は `exponent_coefficient`、$b$ は `prime_coefficient`）を満たす乗法的関数について $\sum_{i=1}^{n}f(i)$ を返す。内部で 2 成分の素数累積和を作って合成する。`n == 0` では $0$ を返し、保証する上限を超えると `std::invalid_argument` を送出する。

## アルゴリズム

$\lfloor N/i\rfloor$ の相異なる $O(\sqrt N)$ 個の値だけを保持し、素数の個数と素数の和をエラトステネスの篩の要領で更新する。その後、最小素因数とその指数を固定する DFS で素数冪を列挙する。$p^3$ が残りの上限を超える枝は素数累積和を使って一括で処理するので、すべての整数やすべての合成数を列挙することはない。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `sum_multiplicative_function_large_max_n` | $O(1)$ | $O(1)$ |
| `LinearPrimePowerMultiplicativeFunction::operator()` | $O(1)$ | $O(1)$ |
| コンストラクタ | $O(\sqrt n\log\log n)$ | オブジェクトに $O(\sqrt n/\log n)$ |
| `limit` / `quotient_table_size` | $O(1)$ | $O(1)$ |
| `quotient_values` | $O(\sqrt n)$ | $O(\sqrt n)$ |
| `prime_prefix_components` | $O(n^{3/4}/\log n)$ | $O(\sqrt n)$ |
| `sum` | $O(n^{3/4}/\log n)$ | 再帰スタック $O(\log n)$、入力表 $O(\sqrt n)$ |
| `sum_of_multiplicative_function_large` | $O(n^{3/4}/\log n)$ | $O(\sqrt n)$ |

## 注意点

- `MOD >= 1` でなければならない（`static_assert` で検査する）。
- `function(p, e)` は `Modint<MOD>` に変換できる値を返す必要がある。
- `prime_prefix` は、同じ `n` で構築した総和クラスが返す商の配置に合わせる。
- `n <= sum_multiplicative_function_large_max_n` が必要である。
