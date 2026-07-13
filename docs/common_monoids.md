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

`GcdMonoid`、`LcmMonoid`、bitwise 3種は整数型用である。

## 長さを使わない作用

| 型 | 区間値 | 作用 | 作用の単位元 | 各操作の計算量 |
| --- | --- | --- | --- | --- |
| `AddMinMonoidAct<T, Identity>` | min | 加算 | `0` | $O(1)$ |
| `AddMaxMonoidAct<T, Identity>` | max | 加算 | `0` | $O(1)$ |
| `AssignMinMonoidAct<T, Identity>` | min | 代入 | 代入なし | $O(1)$ |
| `AssignMaxMonoidAct<T, Identity>` | max | 代入 | 代入なし | $O(1)$ |
| `ChminMinMonoidAct<T, Identity>` | min | chmin | `Identity` | $O(1)$ |
| `ChmaxMaxMonoidAct<T, Identity>` | max | chmax | `Identity` | $O(1)$ |

代入作用の型は `MonoidAssignment<T>` である。

```cpp
MonoidAssignment<long long> none{};          // 代入なし
MonoidAssignment<long long> set_five{true, 5};
```

## 長さを使う作用

| 型 | 区間値 | 作用 | 作用の型・単位元 | 各操作の計算量 |
| --- | --- | --- | --- | --- |
| `AddSumMonoidAct<T>` | sum | 各要素へ加算 | `T(0)` | $O(1)$ |
| `MulSumMonoidAct<T>` | sum | 各要素へ乗算 | `T(1)` | $O(1)$ |
| `AssignSumMonoidAct<T>` | sum | 各要素を代入 | `MonoidAssignment<T>{}` | $O(1)$ |
| `AffineSumMonoidAct<T>` | sum | 各要素を `a*x+b` | `MonoidAffine<T>{1, 0}` | $O(1)$ |
| `FlipCountMonoidAct<T>` | 1 の個数 | 0/1 を反転 | `false` | $O(1)$ |
| `XorXorMonoidAct<T>` | xor | 各要素へ xor | `T(0)` | $O(1)$ |

`MonoidAffine<T>` は `multiplier` と `addend` を持つ。

```cpp
MonoidAffine<long long> f{2, 3}; // x -> 2*x+3
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

## 制約と注意

- min/max の加算・代入・chmin/chmax 作用は `Identity` を空区間の番兵として保存する。
  元の列の実値および更新結果に `Identity` が現れないようにする。
- 長さ付き作用へ渡す長さは 0 以上であり、値はその長さの区間集約値である必要がある。
- 加算、乗算、lcm、affine などの結果が `T` で表現できる範囲に収まるようにする。
- `FlipCountMonoidAct` の区間値は `0 <= count <= length` を満たす。
- 作用付き alias の「各操作」は `op`、`e`、`mapping`、`composition`、`id`
  を指す。補助型 `MonoidAssignment`、`MonoidAffine` の構築と比較も $O(1)$。
- 表の $O(1)$ は `T` の対応する算術・比較・構築を $O(1)$ としたもの。
  一般の `T` では、各aliasが呼ぶ `T` の操作コストに従う。
- `MonoidAssignment` と `MonoidAffine` の各公開field参照は $O(1)$。
