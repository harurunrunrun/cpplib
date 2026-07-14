---
title: Common Monoids (代表的なモノイド)
documentation_of: ../src/structure/types/common_monoids.hpp
---

よく使う monoid と作用の定義。

ここにある型は独自 wrapper ではない。`AddMonoid` などは `Monoid`、
`AddMinMonoidAct` などは `Monoid_Act`、`AddSumMonoidAct` などは
`Monoid_Act_Len` の alias である。そのため、それぞれを要求する
`Segtree`、`LazySegtree`、`DynamicLazySegtree` などへそのまま渡せる。

## 基本 monoid

| 型 | `op` | `e` | `op` の計算量 | `e` の計算量 |
| --- | --- | --- | --- | --- |
| `AddMonoid<T>` | 和 | `0` | $O(1)$ | $O(1)$ |
| `MulMonoid<T>` | 積 | `1` | $O(1)$ | $O(1)$ |
| `MinMonoid<T, Identity>` | min | `Identity`（既定値は `numeric_limits<T>::max()`） | $O(1)$ | $O(1)$ |
| `MaxMonoid<T, Identity>` | max | `Identity`（既定値は `numeric_limits<T>::lowest()`） | $O(1)$ | $O(1)$ |
| `GcdMonoid<T>` | gcd | `0` | $O(\log\max(|a|,|b|))$ | $O(1)$ |
| `LcmMonoid<T>` | lcm | `1` | $O(\log\max(|a|,|b|))$ | $O(1)$ |
| `XorMonoid<T>` | bitwise xor | `0` | $O(1)$ | $O(1)$ |
| `BitAndMonoid<T>` | bitwise and | `~T(0)` | $O(1)$ | $O(1)$ |
| `BitOrMonoid<T>` | bitwise or | `0` | $O(1)$ | $O(1)$ |
| `NonEmptyMaxSubarrayMonoid<T>` | 非空最大部分配列aggregateの結合 | `empty == true` | $O(1)$ | $O(1)$ |
| `AffineCompositionMonoid<T>` | affine関数を列順に合成 | 恒等関数 | $O(1)$ | $O(1)$ |

`GcdMonoid`、`LcmMonoid`、bitwise 3種は整数型用である。
`AffineCompositionMonoid<T>` は左の関数を先、右の関数を後に適用する
非可換monoidである。値型は `MonoidAffine<T>`。


### `NonEmptyMaxSubarrayMonoid<T>`

非空最大部分配列を表す `Monoid` alias。区間値
`NonEmptyMaxSubarrayAggregate<T>` は次の公開fieldを持つ。

| field | 意味 | 参照の計算量 |
| --- | --- | --- |
| `empty` | 空列単位元なら `true` | $O(1)$ |
| `sum` | 非空列の全要素の和 | $O(1)$ |
| `prefix` | 非空最大prefix和 | $O(1)$ |
| `suffix` | 非空最大suffix和 | $O(1)$ |
| `best` | 非空最大部分配列和 | $O(1)$ |

`non_empty_max_subarray_singleton(value)` は1要素aggregate
`{false,value,value,value,value}` を $O(1)$ 時間・空間で返す。
`op` は列の順序を保存する非可換な結合で $O(1)$、`e` は
`empty == true` の空列で $O(1)$ である。空列単位元の数値fieldは0だが、
意味を持たない。

```cpp
constexpr NonEmptyMaxSubarrayMonoid<long long> non_empty_max_subarray{};
auto value = non_empty_max_subarray_singleton(-3LL);
```

## 長さを使わない作用

| 型 | 区間値 | 作用 | 作用の単位元 | 各操作の計算量 |
| --- | --- | --- | --- | --- |
| `AddMinMonoidAct<T, Identity>` | min | 加算 | `0` | $O(1)$ |
| `AddMaxMonoidAct<T, Identity>` | max | 加算 | `0` | $O(1)$ |
| `AssignMinMonoidAct<T, Identity>` | min | 代入 | 代入なし | $O(1)$ |
| `AssignMaxMonoidAct<T, Identity>` | max | 代入 | 代入なし | $O(1)$ |
| `ChminMinMonoidAct<T, Identity>` | min | chmin | `Identity` | $O(1)$ |
| `ChmaxMaxMonoidAct<T, Identity>` | max | chmax | `Identity` | $O(1)$ |
| `AddHistoricalMaxMonoidAct<T>` | 現在値のmax・過去に到達した値のmax | 各要素へ加算 | `HistoricalAdd<T>{0,0}` | $O(1)$ |
| `AffineSumAggregateMonoidAct<T>` | 和と長さ | 各要素を `a*x+b` | `MonoidAffine<T>{1,0}` | $O(1)$ |
| `AssignAffineCompositionMonoidAct<T>` | affine合成と長さ | 全関数を一様代入 | `MonoidAssignment<MonoidAffine<T>>{}` | mappingは $O(\log length)$、ほかは $O(1)$ |

`HistoricalMaxAggregate<T>` は次の公開fieldを持つ。

| field | 意味 | 参照の計算量 |
| --- | --- | --- |
| `current` | 現在値の区間最大値 | $O(1)$ |
| `historical` | 初期化以降に到達した値の区間最大値 | $O(1)$ |

`historical_max_singleton(value)` は両fieldが `value` の1要素aggregateを
$O(1)$ 時間・空間で返す。monoidの空区間単位元では両fieldが
`numeric_limits<T>::lowest()` になる。

`HistoricalAdd<T>` は作用列について次の公開fieldを持つ。

| field | 意味 | 参照の計算量 |
| --- | --- | --- |
| `total` | 作用列全体の加算量 | $O(1)$ |
| `peak` | 空prefixを含む作用列の最大prefix加算量 | $O(1)$ |

`historical_add(delta)` は1回の加算を表す
`{delta, max(T(0), delta)}` を $O(1)$ 時間・空間で返す。
`composition(newer, older)` は先に `older`、後に `newer` を適用し、

```cpp
{
    older.total + newer.total,
    max(older.peak, older.total + newer.peak)
}
```

を返す。`op`、`e`、`mapping`、`composition`、`id` はすべて $O(1)$ 時間・
$O(1)$ 追加空間である。

代入作用の型は `MonoidAssignment<T>` である。

```cpp
MonoidAssignment<long long> none{};          // 代入なし
MonoidAssignment<long long> set_five{true, 5};
```

`SumLengthAggregate<T>` は公開field `sum` と `length` を持ち、区間和と区間長を
表す。`AffineSumAggregateMonoidAct<T>` の値型として使う。field参照、構築、比較、
`op`、`e`、`mapping`、`composition`、`id` はすべて $O(1)$。

`AffineCompositionAggregate<T>` は公開field `affine` と `length` を持ち、
区間の関数合成と区間長を表す。`AssignAffineCompositionMonoidAct<T>` の値型である。
`op`、`e`、`composition`、`id` は $O(1)$。`mapping` は代入するaffine関数を
`length` 回合成するため $O(\log length)$、追加空間 $O(1)$ である。

## 長さを使う作用

| 型 | 区間値 | 作用 | 作用の型・単位元 | 各操作の計算量 |
| --- | --- | --- | --- | --- |
| `AddSumMonoidAct<T>` | sum | 各要素へ加算 | `T(0)` | $O(1)$ |
| `MulSumMonoidAct<T>` | sum | 各要素へ乗算 | `T(1)` | $O(1)$ |
| `AssignSumMonoidAct<T>` | sum | 各要素を代入 | `MonoidAssignment<T>{}` | $O(1)$ |
| `AffineSumMonoidAct<T>` | sum | 各要素を `a*x+b` | `MonoidAffine<T>{1, 0}` | $O(1)$ |
| `FlipCountMonoidAct<T>` | 1 の個数 | 0/1 を反転 | `false` | $O(1)$ |
| `XorXorMonoidAct<T>` | xor | 各要素へ xor | `T(0)` | $O(1)$ |
| `AssignMaxSubarrayMonoidAct<T>` | 和・最大prefix/suffix・最大部分配列和 | 各要素を代入 | `MonoidAssignment<T>{}` | $O(1)$ |

`MonoidAffine<T>` は `multiplier` と `addend` を持つ。

```cpp
MonoidAffine<long long> f{2, 3}; // x -> 2*x+3
```

`MaxSubarrayAggregate<T>` は、列について次の4値を保持する公開aggregate型である。

| field | 意味 | 参照の計算量 |
| --- | --- | --- |
| `sum` | 全要素の和 | $O(1)$ |
| `prefix` | 空列を許す最大prefix和 | $O(1)$ |
| `suffix` | 空列を許す最大suffix和 | $O(1)$ |
| `best` | 空列を許す最大部分配列和 | $O(1)$ |

`max_subarray_singleton(value)` は要素1個のaggregateを $O(1)$ 時間・空間で作る。
負の1要素に対する `prefix`、`suffix`、`best` はすべて0になる。
`AssignMaxSubarrayMonoidAct<T>` の `op` は列の順序を保存する非可換な結合であり、
`mapping` は区間全体への一様代入を反映する。逆向きaggregateも保持する
`LazyLinkCutTree` のpath query/updateにもそのまま利用できる。

```cpp
constexpr AssignMaxSubarrayMonoidAct<long long> max_subarray_assign{};
auto value = max_subarray_singleton(-3LL);
auto assigned = MonoidAssignment<long long>{true, 5};
```

すべての作用で
`composition(f, g)` は「先に `g`、後に `f`」、
すなわち `f(g(x))` を表す。例えば affine 作用では次のようになる。

```cpp
constexpr AffineSumMonoidAct<long long> act{};
static_assert(
    act.composition({2, 5}, {3, 7})
    == MonoidAffine<long long>{6, 19}
);
```

## 使い方

```cpp
constexpr AddMonoid<long long> add_monoid{};
Segtree<add_monoid, 200000> seg(n);

constexpr AddMinMonoidAct<long long> add_min{};
LazySegtree<add_min, 200000> lazy_seg(n);

constexpr AffineSumMonoidAct<long long> affine_sum{};
DynamicLazySegtree<affine_sum, 200000> dynamic_seg(n);
```

min/max 系の単位元は非型テンプレート引数で変更できる。

```cpp
constexpr MinMonoid<long long, (1LL << 60)> min_monoid{};
constexpr AddMinMonoidAct<long long, (1LL << 60)> add_min{};
```

## 注意点

- min/max の加算・代入・chmin/chmax 作用は `Identity` を空区間の番兵として保存する。
  元の列の実値および更新結果に `Identity` が現れないようにする。
- 長さ付き作用へ渡す長さは 0 以上であり、値はその長さの区間集約値である必要がある。
- 加算、乗算、lcm、affine などの結果が `T` で表現できる範囲に収まるようにする。
- `FlipCountMonoidAct` の区間値は `0 <= count <= length` を満たす。
- `MaxSubarrayAggregate<T>` は `T(0)`、加算、乗算、`std::max` に対応する
  `T` を要求する。各fieldは `max_subarray_singleton`、作用の `e` / `op` /
  `mapping` で作り、不変条件を満たさないfield値を直接渡さない。
- `AssignMaxSubarrayMonoidAct<T>` では、すべての部分和および
  `assigned.value * T(length)` が `T` で表現できる必要がある。
  符号付き整数のoverflowは未定義動作になる。
- 最大部分配列和は空部分配列を許すため常に0以上であり、全要素が負なら
  `best == 0` となる。非空部分配列を要求する用途とは契約が異なる。
- 長さ0への代入mappingは単位元 `{0,0,0,0}` を返す。
- 作用付き alias の「各操作」は `op`、`e`、`mapping`、`composition`、`id`
  を指す。補助型の構築と比較も $O(1)$。
- 表の $O(1)$ は `T` の対応する算術・比較・構築を $O(1)$ としたもの。
  一般の `T` では、各aliasが呼ぶ `T` の操作コストに従う。
- 各補助型の公開field参照は $O(1)$。
- `NonEmptyMaxSubarrayAggregate<T>` の `sum`、`prefix`、`suffix`、`best` と、
  `non_empty_max_subarray_op` が計算するすべての部分和は `T` で表現できなければならない。
- historical maximum関連APIの `T` は、`numeric_limits<T>` が特殊化された符号付き型で
  なければならない。`numeric_limits<T>::lowest()` は空区間の番兵なので、実値の
  `current` / `historical` に使用しない。
- `HistoricalMaxAggregate<T>` は通常 `historical >= current` を満たす値を渡す。
  `HistoricalAdd<T>` を直接作る場合、`peak` は作用列の空prefixを含む最大prefix和であり、
  `peak >= 0` かつ `peak >= total` を満たさなければならない。1回の加算には
  `historical_add(delta)` を使う。
- historical加算の `current + total`、`current + peak`、`total` 同士の加算、
  `total + peak` はすべて `T` で表現できなければならない。符号付き整数のoverflowは
  未定義動作になる。

## helper API

`common_monoid_internal` にある次の関数が各aliasを構成する。通常はalias経由で使う。

| 関数 | 操作 | 時間計算量 |
| --- | --- | --- |
| `add_op(left,right)` | 加算 | $O(1)$ |
| `multiply_op(left,right)` | 乗算 | $O(1)$ |
| `minimum_op(left,right)` | 小さい方 | $O(1)$ |
| `maximum_op(left,right)` | 大きい方 | $O(1)$ |
| `gcd_op(left,right)` | 最大公約数 | $O(\log\max(|left|,|right|))$ |
| `lcm_op(left,right)` | 最小公倍数 | $O(\log\max(|left|,|right|))$ |
| `xor_op(left,right)` | bitwise xor | $O(1)$ |
| `bit_and_op(left,right)` | bitwise and | $O(1)$ |
| `bit_or_op(left,right)` | bitwise or | $O(1)$ |
| `zero<T>()` | `T(0)` | $O(1)$ |
| `one<T>()` | `T(1)` | $O(1)$ |
| `all_bits<T>()` | `~T(0)` | $O(1)$ |
| `constant<T,Value>()` | template値 `Value` | $O(1)$ |
| `add_extremum_mapping<T,Identity>(f,x)` | 番兵を保ったmin/max値への加算 | $O(1)$ |
| `add_composition(f,g)` | 加算作用の合成 | $O(1)$ |
| `assign_extremum_mapping<T,Identity>(f,x)` | 番兵を保ったmin/max値への代入 | $O(1)$ |
| `assignment_composition(f,g)` | 新しい代入を優先して合成 | $O(1)$ |
| `assignment_id<T>()` | 代入なし作用 | $O(1)$ |
| `sum_op(left,llen,right,rlen)` | 長さ付き区間和の結合 | $O(1)$ |
| `add_sum_mapping(f,x,length)` | 区間和へ一様加算を反映 | $O(1)$ |
| `assign_sum_mapping(f,x,length)` | 区間和へ一様代入を反映 | $O(1)$ |
| `affine_sum_mapping(f,x,length)` | 区間和へaffine作用を反映 | $O(1)$ |
| `affine_composition(f,g)` | `f(g(x))` のaffine作用 | $O(1)$ |
| `affine_id<T>()` | 恒等affine作用 | $O(1)$ |
| `affine_sequence_op(left,right)` | 左を先、右を後に適用するaffine合成 | $O(1)$ |
| `sum_length_op(left,right)` | 和・長さaggregateの結合 | $O(1)$ |
| `sum_length_identity<T>()` | 和0・長さ0のaggregate | $O(1)$ |
| `affine_sum_length_mapping(f,x)` | 和・長さaggregateへaffine作用を反映 | $O(1)$ |
| `affine_power(base,exponent)` | affine関数の反復合成 | $O(\log exponent)$ |
| `affine_aggregate_op(left,right)` | affine合成・長さaggregateの結合 | $O(1)$ |
| `affine_aggregate_identity<T>()` | 恒等関数・長さ0のaggregate | $O(1)$ |
| `assign_affine_mapping(f,x)` | 一様代入後のaffine合成を反映 | $O(\log x.length)$ |
| `xor_len_op(left,llen,right,rlen)` | 長さ付きxorの結合 | $O(1)$ |
| `xor_xor_mapping(f,x,length)` | 区間xorへ一様xorを反映 | $O(1)$ |
| `chmin_mapping<T,Identity>(f,x)` | 番兵を保って `min(f,x)` | $O(1)$ |
| `chmax_mapping<T,Identity>(f,x)` | 番兵を保って `max(f,x)` | $O(1)$ |
| `multiply_sum_mapping(f,x,length)` | 区間和へ一様乗算を反映 | $O(1)$ |
| `flip_count_mapping(f,x,length)` | 0/1反転後の1の個数 | $O(1)$ |
| `max_subarray_op(left,llen,right,rlen)` | 2列のaggregateを順に結合 | $O(1)$ |
| `max_subarray_identity<T>()` | 空列aggregate `{0,0,0,0}` | $O(1)$ |
| `assign_max_subarray_mapping(f,x,length)` | 一様代入後のaggregate | $O(1)$ |
| `non_empty_max_subarray_op(left,right)` | 2つの非空最大部分配列aggregateを列順に結合 | $O(1)$ |
| `non_empty_max_subarray_identity<T>()` | `empty == true` の空列単位元 | $O(1)$ |
| `historical_max_op(left,right)` | current/historicalをそれぞれmaxで結合 | $O(1)$ |
| `historical_max_identity<T>()` | 両fieldが `numeric_limits<T>::lowest()` の単位元 | $O(1)$ |
| `historical_add_mapping(f,x)` | 加算作用列をcurrent/historicalへ反映 | $O(1)$ |
| `historical_add_composition(f,g)` | 先に `g`、後に `f` の加算作用列を合成 | $O(1)$ |
| `historical_add_identity<T>()` | 加算作用列の単位元 `{0,0}` | $O(1)$ |
| `bool_xor(f,g)` | bool作用のxor合成 | $O(1)$ |
| `false_value()` | bool作用の単位元 `false` | $O(1)$ |

全helperの追加空間計算量は $O(1)$。表の計算量は `T` の各演算を $O(1)$ とした値である。
`gcd_op`, `lcm_op`, bitwise helper、xor作用は整数型だけに使用できる。

## 補助型API

- `MonoidAssignment<T>::assigned`：代入作用を持つかを表す。既定値は `false`。
- `MonoidAssignment<T>::value`：`assigned == true` のときの代入値。
- `operator==(MonoidAssignment, MonoidAssignment)`：両方が未代入なら `value` によらず
  等しい。代入ありならflagと値を比較する。
- `MonoidAffine<T>::multiplier` / `addend`：`x -> multiplier*x+addend` を表し、既定値は
  `1` / `0`。
- `operator==(MonoidAffine, MonoidAffine)`：2つのfieldを比較する。
- `SumLengthAggregate<T>::sum` / `length`：区間和と0以上の区間長。
- `operator==(SumLengthAggregate, SumLengthAggregate)`：2つのfieldを比較する。
- `AffineCompositionAggregate<T>::affine` / `length`：列順の関数合成と
  0以上の区間長。
- `operator==(AffineCompositionAggregate, AffineCompositionAggregate)`：
  2つのfieldを比較する。
- `AffineCompositionMonoid<T>`：`MonoidAffine<T>` を列順に合成する
  `Monoid` alias。
- `AffineSumAggregateMonoidAct<T>`：`SumLengthAggregate<T>` へaffine作用を行う
  `Monoid_Act` alias。
- `AssignAffineCompositionMonoidAct<T>`：`AffineCompositionAggregate<T>` へ
  affine関数の一様代入を行う `Monoid_Act` alias。
- `MaxSubarrayAggregate<T>::sum` / `prefix` / `suffix` / `best`：
  列全体の和、空列を許す最大prefix和、最大suffix和、最大部分配列和を表す。
- `operator==(MaxSubarrayAggregate, MaxSubarrayAggregate)`：4つのfieldを比較する。
- `max_subarray_singleton(value)`：1要素列のaggregateを返す。
- `AssignMaxSubarrayMonoidAct<T>`：`MaxSubarrayAggregate<T>` に
  `MonoidAssignment<T>` を作用させる `Monoid_Act_Len` alias。
- `NonEmptyMaxSubarrayAggregate<T>::empty`：monoidの空列単位元なら `true`。
- `NonEmptyMaxSubarrayAggregate<T>::sum` / `prefix` / `suffix` / `best`：
  非空列の全体和、非空最大prefix和、非空最大suffix和、非空最大部分配列和。
- `operator==(NonEmptyMaxSubarrayAggregate, NonEmptyMaxSubarrayAggregate)`：
  `empty` と4つの値fieldを比較する。
- `non_empty_max_subarray_singleton(value)`：1要素の非空aggregateを返す。
- `NonEmptyMaxSubarrayMonoid<T>`：上記aggregateを列順に結合する `Monoid` alias。
- `HistoricalMaxAggregate<T>::current`：現在値の区間最大値。
- `HistoricalMaxAggregate<T>::historical`：初期化以降に到達した値の区間最大値。
- `operator==(HistoricalMaxAggregate, HistoricalMaxAggregate)`：2つのfieldを比較する。
- `historical_max_singleton(value)`：`current` と `historical` がともに `value` の
  aggregateを返す。
- `HistoricalAdd<T>::total`：作用列をすべて適用したときの加算量。
- `HistoricalAdd<T>::peak`：空prefixを含む作用列の最大prefix加算量。
- `operator==(HistoricalAdd, HistoricalAdd)`：2つのfieldを比較する。
- `historical_add(delta)`：1回の加算を表す `{delta, max(0, delta)}` を返す。
- `AddHistoricalMaxMonoidAct<T>`：historical maximumへ区間加算を作用させる
  `Monoid_Act` alias。

各field参照、aggregate構築、比較の時間・追加空間計算量は $O(1)$。各objectの保存領域は
`T` 1個、2個、または4個とflagに対して $O(1)$。
