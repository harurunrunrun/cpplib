---
title: Sum Squares Monoid Act (二乗和作用付きモノイド) [SEGSQRSS]
documentation_of: ../src/structure/types/sum_squares_monoid.hpp
---

数列の要素和と二乗和を保持し、区間へのaffine変換
`x -> multiplier * x + addend`を扱う作用付きモノイド。
`AffineSumSquaresMonoidAct<T>`は独自containerではなく、既存の`Monoid_Act`の
aliasであり、`LazySegtree`などへ渡して利用できる。

# `SumSquaresAggregate<T>`

```cpp
template<class T>
struct SumSquaresAggregate {
    T sum;
    T square_sum;
    long long length;
    friend constexpr bool operator==(
        const SumSquaresAggregate&,
        const SumSquaresAggregate&
    );
};
```

区間`x[0], ..., x[length - 1]`を次の3個の公開fieldで表す。

| field | 意味 | 参照の時間計算量 |
| --- | --- | --- |
| `sum` | $\sum_i x_i$ | $O(1)$ |
| `square_sum` | $\sum_i x_i^2$ | $O(1)$ |
| `length` | 区間長 | $O(1)$ |

default構築した値は`{T{}, T{}, 0}`であり、空区間を表す単位元と同じである。
`operator==`は3個のfieldを比較する。default構築と比較はいずれも$O(1)$時間、
$O(1)$追加空間である。

# `sum_squares_singleton`

```cpp
template<class T>
constexpr SumSquaresAggregate<T> sum_squares_singleton(T value);
```

1要素列`[value]`を表す`{value, value * value, 1}`を返す。
時間計算量と追加空間計算量は$O(1)$である。

# 作用を作るhelper

## `sum_squares_assign`

```cpp
template<class T>
constexpr MonoidAffine<T> sum_squares_assign(T value);
```

すべての要素を`value`へ代入する作用`{0, value}`を返す。
時間計算量と追加空間計算量は$O(1)$である。

## `sum_squares_add`

```cpp
template<class T>
constexpr MonoidAffine<T> sum_squares_add(T delta);
```

すべての要素へ`delta`を加える作用`{1, delta}`を返す。
時間計算量と追加空間計算量は$O(1)$である。

# `AffineSumSquaresMonoidAct<T>`

```cpp
template<class T>
using AffineSumSquaresMonoidAct = Monoid_Act<...>;
```

値型`S`は`SumSquaresAggregate<T>`、作用型`T`は`MonoidAffine<T>`である。
ここで後者のtemplate引数との混同を避けるため、以下では作用を`f = {a, b}`と
書く。`f`は各要素を`a * x + b`へ変換する。

| API | 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- | --- |
| `op(left, right)` | 対応する3 fieldを加算して区間を連結 | $O(1)$ | $O(1)$ |
| `e()` | `{T{}, T{}, 0}`の空区間を返す | $O(1)$ | $O(1)$ |
| `mapping(f, value)` | aggregateの全要素へaffine変換を適用 | $O(1)$ | $O(1)$ |
| `composition(newer, older)` | `older`の後に`newer`を適用する作用を合成 | $O(1)$ | $O(1)$ |
| `id()` | `{1, 0}`の恒等作用を返す | $O(1)$ | $O(1)$ |

`value = {sum, square_sum, length}`に`f = {a, b}`を適用すると、返値は次になる。

$$
\begin{aligned}
sum' &= a\,sum+b\,length,\\
square\_sum' &= a^2\,square\_sum+2ab\,sum+b^2\,length,\\
length' &= length.
\end{aligned}
$$

`composition(f, g)`は`f(g(x))`、すなわち次の作用を返す。

$$
\{f.a * g.a,\ f.a * g.b + f.b\}.
$$

`op`は左右の順序によらず同じ値になるが、区間連結として`left`、`right`の順に
長さを加算する。

```cpp
constexpr AffineSumSquaresMonoidAct<long long> action{};
std::vector<SumSquaresAggregate<long long>> initial;
for(long long value: values){
    initial.push_back(sum_squares_singleton(value));
}
LazySegtree<action, 100000> tree(initial);

tree.apply(left, right, sum_squares_assign(5LL));
tree.apply(left, right, sum_squares_add(-2LL));
long long answer = tree.prod(left, right).square_sum;
```

モノイド演算と作用が$O(1)$なので、`LazySegtree`上の区間更新と区間取得は
$O(\log N)$時間、tree全体は$O(N)$空間になる。

# 実装用関数

aliasは`sum_squares_monoid_internal`の次の関数と、
`common_monoid_internal`にある既存のaffine作用合成から構成される。

| 関数 | 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- | --- |
| `sum_squares_op<T>(left, right)` | aggregateの連結 | $O(1)$ | $O(1)$ |
| `sum_squares_identity<T>()` | 空区間の構築 | $O(1)$ | $O(1)$ |
| `affine_sum_squares_mapping<T>(operation, value)` | affine作用の適用 | $O(1)$ | $O(1)$ |
| `common_monoid_internal::affine_composition<T>(f, g)` | `f(g(x))`の合成 | $O(1)$ | $O(1)$ |
| `common_monoid_internal::affine_id<T>()` | affine恒等作用 | $O(1)$ | $O(1)$ |

通常はこれらを直接呼ばず、公開aliasの`op`、`e`、`mapping`、`composition`、
`id`を使う。

# 注意点

- `T{}`、`T(0)`が加法単位元、`T(1)`が乗法単位元を表し、`T(2)`と
  `static_cast<T>(length)`を構築できる必要がある。`T`の加算と乗算には通常の
  可換な算術法則と分配法則を要求する。
- aggregateを直接構築・変更する場合、`length >= 0`であり、`sum`と
  `square_sum`が同じ`length`個の列を表すという不変条件を利用側が保つ。
- `op`での`length`の加算結果は`long long`に収まる必要がある。
- `sum_squares_singleton`の`value * value`、`op`の各加算、`mapping`の
  $a^2\,square\_sum$、$2ab\,sum$、$b^2\,length$とそれらの加算、作用合成の
  各途中結果がすべて`T`で表現できる必要がある。数学的な最終値だけでなく、
  C++で評価される途中値にもこの条件を課す。
- signed整数型のoverflowは未定義動作である。`long long`を使う場合も、更新後の
  要素、要素和、二乗和、および前項の各途中結果が範囲内であることを保証する。
  unsigned整数型は型のbit幅を法としてwrapし、mod整数型ではその法で計算する。
- `composition(newer, older)`は先に`older`、後に`newer`を適用する。
  `sum_squares_assign(x)`はそれ以前の作用を上書きし、
  `sum_squares_add(x)`はそれ以前の作用結果へ加算する。
