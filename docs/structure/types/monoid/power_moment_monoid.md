---
title: Power Moment Monoid (位置冪モーメントモノイド)
documentation_of: ../../../../src/structure/types/monoid/power_moment_monoid.hpp
---

列の各要素に、その列内での 1 始まりの位置の冪を掛けた和を保持するモノイドである。
列 `x[0], ..., x[length - 1]` に対応する集約値は、各
`0 <= p <= MAX_POWER` について次を保持する。

$$
\mathtt{moment}[p]=\sum_{i=0}^{\mathtt{length}-1}x[i](i+1)^p.
$$

位置は集約値ごとに 1 から数え直す。このため区間積から、その区間の左端を
基準とする位置冪和を直接得られる。

## `PowerMomentAggregate`

```cpp
template<class T, size_t MAX_POWER>
struct PowerMomentAggregate {
    size_t length;
    array<T, MAX_POWER + 1> moment;
};
```

- `length`：集約値が表す列の長さ。
- `moment[p]`：上式の $p$ 次モーメント。`moment[0]` は通常の要素和になる。
- `operator==`：`length` とすべての `moment` が等しいかを比較する。

デフォルト構築した値は長さ 0、全モーメント 0 であり、モノイドの単位元と同じである。
フィールドの参照と、比較の 1 要素あたりの時間計算量は $O(1)$、集約値全体の比較は
$O(\texttt{MAX_POWER})$。

## `power_moment_singleton`

```cpp
template<class T, size_t MAX_POWER>
PowerMomentAggregate<T, MAX_POWER>
power_moment_singleton(const T& value);
```

1 要素列 `[value]` を表す集約値を返す。位置が 1 なので、すべての
`moment[p]` は `value` になる。

## 時間計算量

- 時間計算量：$O(\texttt{MAX_POWER})$
- 返り値の空間：$O(\texttt{MAX_POWER})$

## `PowerMomentMonoid`

```cpp
template<class T, size_t MAX_POWER>
using PowerMomentMonoid = Monoid<...>;
```

共通の `Monoid` 型に、位置冪モーメント用の演算と単位元を設定したエイリアスである。
`using S = PowerMomentAggregate<T, MAX_POWER>`、`S op(S, S) const`、
`S e() const` を公開する。

`op(left, right)` は `left` の列の直後に `right` の列を連結した集約値を返す。
`L = left.length` とすると、右側の位置を二項展開して次のように結合する。

$$
\operatorname{result}_p
=\operatorname{left}_p
+\sum_{j=0}^{p}\binom pj L^{p-j}\operatorname{right}_j.
$$

列の順序を保存する非可換モノイドである。`e()` は長さ 0、全モーメント 0 の
単位元を返す。

## 時間計算量

- `op`：$O(\texttt{MAX_POWER}^2)$
- `e`：$O(\texttt{MAX_POWER})$
- 集約値 1 個の空間：$O(\texttt{MAX_POWER})$
- テンプレートの実体ごとの二項係数表：$O(\texttt{MAX_POWER}^2)$

## 内部の補助関数

`power_moment_detail` 名前空間には、エイリアスを構成する次の実装詳細がある。

- `binomial<MAX_POWER>`：コンパイル時に構築する二項係数表。
- `power_moment_op<T, MAX_POWER>(left, right)`：上記の連結演算。
- `power_moment_identity<T, MAX_POWER>()`：長さ 0 の単位元。

これらは `PowerMomentMonoid` から利用される内部 API であり、利用側は直接参照せず、
公開エイリアスの `op` と `e` を使う。各内部補助関数の計算量は前節の対応する公開操作と同じ。

## 使用例

```cpp
using value_type = std::uint32_t;
constexpr std::size_t max_power = 10;
constexpr PowerMomentMonoid<value_type, max_power> monoid{};

auto left = power_moment_singleton<value_type, max_power>(3);
auto right = power_moment_singleton<value_type, max_power>(5);
auto aggregate = monoid.op(left, right);

// 3 * 1^2 + 5 * 2^2 = 23
assert(aggregate.moment[2] == 23);
```

`ReversibleSplayTree` のモノイドとして使うと、挿入・削除・置換・反転と
区間位置冪和を扱える。モノイド演算の時間を含め、各列操作は最悪
$O(N\texttt{MAX_POWER}^2)$、操作列に対してならし
$O(\texttt{MAX_POWER}^2\log(N+1))$ となる。

## 注意点

- `0 <= MAX_POWER <= 67` を要求する。二項係数は `std::uint64_t` でコンパイル時に
  保持し、使用時に `T` へ変換する。
- `T{}` が加法単位元、`T(1)` が乗法単位元であり、`T` が長さと二項係数から
  構築できることを要求する。加算・乗算は結合的かつ分配的で、乗算は可換で
  なければならない。
- `left.length + right.length` が `std::size_t` に収まることを要求する。
- `T = std::uint32_t` では、C++ の符号なし整数の加算・乗算が $2^{32}$ を法として
  ラップアラウンドする。
- 符号付き整数型のオーバーフローは未定義動作である。途中結果が型に収まることを
  保証するか、符号なし整数型または剰余環の整数型を使う。
- `moment[p]` の位置は常に集約値の左端から 1 始まりである。絶対的な添字を
  用いる値ではない。
