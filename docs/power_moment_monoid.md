---
title: Power Moment Monoid (位置冪モーメントモノイド)
documentation_of: ../src/structure/types/power_moment_monoid.hpp
---

列の各要素に、その列内での1始まりの位置の冪を掛けた和を保持するモノイド。
列 `x[0], ..., x[length - 1]` に対応するaggregateは、各
`0 <= p <= MAX_POWER`について次を保持する。

$$
\mathtt{moment}[p]=\sum_{i=0}^{\mathtt{length}-1}x[i](i+1)^p.
$$

位置はaggregateごとに1から数え直す。このため区間積から、その区間の左端を
基準とする位置冪和を直接得られる。

# `PowerMomentAggregate`

```cpp
template<class T, size_t MAX_POWER>
struct PowerMomentAggregate {
    size_t length;
    array<T, MAX_POWER + 1> moment;
};
```

- `length`：aggregateが表す列の長さ。
- `moment[p]`：上式の$p$次モーメント。`moment[0]`は通常の要素和になる。
- `operator==`：`length`とすべての`moment`が等しいかを比較する。

default構築した値は長さ0、全モーメント0であり、モノイドの単位元と同じである。
fieldの参照と比較1要素あたりの時間計算量は$O(1)$、aggregate全体の比較は
$O(\mathtt{MAX\_POWER})$。

# `power_moment_singleton`

```cpp
template<class T, size_t MAX_POWER>
PowerMomentAggregate<T, MAX_POWER>
power_moment_singleton(const T& value);
```

1要素列`[value]`を表すaggregateを返す。位置が1なので、すべての
`moment[p]`は`value`になる。

## 時間計算量

- 時間：$O(\mathtt{MAX\_POWER})$
- 返り値の空間：$O(\mathtt{MAX\_POWER})$

# `PowerMomentMonoid`

```cpp
template<class T, size_t MAX_POWER>
using PowerMomentMonoid = Monoid<...>;
```

共通の`Monoid`型へ、位置冪モーメント用の演算と単位元を設定したalias。
`using S = PowerMomentAggregate<T, MAX_POWER>`、`S op(S, S) const`、
`S e() const`を公開する。

`op(left, right)`は`left`の列の直後に`right`の列を連結したaggregateを返す。
`L = left.length`とすると、右側の位置を二項展開して次のように結合する。

$$
\operatorname{result}_p
=\operatorname{left}_p
+\sum_{j=0}^{p}\binom pj L^{p-j}\operatorname{right}_j.
$$

列の順序を保存する非可換モノイドである。`e()`は長さ0、全モーメント0の
単位元を返す。

## 時間計算量

- `op`：$O(\mathtt{MAX\_POWER}^2)$
- `e`：$O(\mathtt{MAX\_POWER})$
- aggregate 1個の空間：$O(\mathtt{MAX\_POWER})$
- template実体ごとの二項係数表：$O(\mathtt{MAX\_POWER}^2)$

# 内部helper

`power_moment_detail`名前空間には、aliasを構成する次の実装詳細がある。

- `binomial<MAX_POWER>`：compile時に構築する二項係数表。
- `power_moment_op<T, MAX_POWER>(left, right)`：上記の連結演算。
- `power_moment_identity<T, MAX_POWER>()`：長さ0の単位元。

これらは`PowerMomentMonoid`から利用される内部APIであり、利用側は直接参照せず、
公開aliasの`op`と`e`を使う。各内部helperの計算量は前節の対応する公開操作と同じ。

# 使用例

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

`ReversibleSplayTree`のモノイドとして使うと、挿入・削除・置換・反転と
区間位置冪和を扱える。モノイド演算の時間を含め、各列操作はworst-case
$O(N\mathtt{MAX\_POWER}^2)$、操作列に対して償却
$O(\mathtt{MAX\_POWER}^2\log(N+1))$となる。

# 注意点

- `0 <= MAX_POWER <= 67`を要求する。二項係数は`std::uint64_t`でcompile時に
  保持し、使用時に`T`へ変換する。
- `T{}`が加法単位元、`T(1)`が乗法単位元であり、`T`が長さと二項係数から
  構築できることを要求する。加算・乗算は結合的かつ分配的で、乗算は可換で
  なければならない。
- `left.length + right.length`が`std::size_t`に収まることを要求する。
- `T = std::uint32_t`では、C++のunsigned加算・乗算が$2^{32}$を法として
  wrapするため、SPOJ GSS8の剰余を追加のmod演算なしで正確に表す。
- signed整数型のoverflowは未定義動作である。途中結果が型に収まることを
  保証するか、unsigned整数型またはmod整数型を使う。
- `moment[p]`の位置は常にaggregateの左端から1始まりである。絶対indexを
  用いる値ではない。
