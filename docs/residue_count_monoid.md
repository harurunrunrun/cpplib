---
title: Residue Count Monoid (剰余個数モノイド) [MULTQ3]
documentation_of: ../src/structure/types/residue_count_monoid.hpp
---

整数列を法 `MOD` の剰余ごとに数え、区間内の全要素への一様加算を扱う作用付き
モノイド。`AddResidueCountMonoidAct<Count, MOD>` は独自containerではなく、既存の
`Monoid_Act` のaliasである。

# `ResidueShift<MOD>`

```cpp
template<int MOD>
struct ResidueShift {
    int shift;
    constexpr ResidueShift();
    constexpr ResidueShift(long long delta);
    friend constexpr bool operator==(const ResidueShift&, const ResidueShift&);
};
```

一様加算作用を表す。default constructorは恒等作用を作る。`delta` を受け取る
constructorは正負の値を数学的なmodで正規化し、公開field `shift` に
`0 <= shift < MOD` を満たす値を保存する。`long long` の最小値と最大値も入力できる。
`operator==` は `shift` を比較する。

constructor、field参照、比較はいずれも $O(1)$ 時間、$O(1)$ 追加空間である。

# `ResidueCountAggregate<Count, MOD>`

```cpp
template<class Count, int MOD>
struct ResidueCountAggregate {
    array<Count, MOD> count;
    friend constexpr bool operator==(
        const ResidueCountAggregate&,
        const ResidueCountAggregate&
    );
};
```

区間の剰余別個数を保持する。公開field `count[r]` は剰余が `r` の要素数である。
field 1個の参照は $O(1)$、aggregate全体のdefault構築と比較は $O(MOD)$ 時間、
$O(1)$ 追加空間である。本体の保存領域は $O(MOD)$。

# `residue_count_singleton`

```cpp
template<class Count, int MOD>
constexpr ResidueCountAggregate<Count, MOD>
residue_count_singleton(long long value);
```

整数 `value` 1個を表すaggregateを返す。負の値も数学的なmodで正規化する。
返値では `count[value mod MOD] == Count(1)`、それ以外は `Count{}` である。

時間計算量は $O(MOD)$、返値を除く追加空間計算量は $O(1)$。

# `AddResidueCountMonoidAct<Count, MOD>`

```cpp
template<class Count, int MOD>
using AddResidueCountMonoidAct = Monoid_Act<...>;
```

`S` は `ResidueCountAggregate<Count, MOD>`、作用型 `T` は
`ResidueShift<MOD>` である。

| API | 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- | --- |
| `op(left, right)` | 対応する剰余の個数を加算 | $O(MOD)$ | $O(MOD)$（返値） |
| `e()` | 全個数が0の空区間 | $O(MOD)$ | $O(MOD)$（返値） |
| `mapping(action, value)` | 各剰余を `action.shift` だけ巡回移動 | $O(MOD)$ | $O(MOD)$（返値） |
| `composition(newer, older)` | `older` の後に `newer` を適用する作用 | $O(1)$ | $O(1)$ |
| `id()` | 加算量0の恒等作用 | $O(1)$ | $O(1)$ |

```cpp
constexpr AddResidueCountMonoidAct<int, 3> action{};
std::vector<ResidueCountAggregate<int, 3>> initial(
    n,
    residue_count_singleton<int, 3>(0)
);
LazySegtree<action, 100000> tree(initial);

tree.apply(left, right, ResidueShift<3>{-1000000000000LL});
int divisible_by_three = tree.prod(left, right).count[0];
```

# 実装用関数

aliasは `residue_count_monoid_internal` の次の関数から構成される。

| 関数 | 時間計算量 |
| --- | --- |
| `residue_count_op<Count, MOD>(left, right)` | $O(MOD)$ |
| `residue_count_identity<Count, MOD>()` | $O(MOD)$ |
| `residue_shift_mapping<Count, MOD>(action, value)` | $O(MOD)$ |
| `residue_shift_composition<MOD>(newer, older)` | $O(1)$ |
| `residue_shift_identity<MOD>()` | $O(1)$ |

通常はこれらを直接呼ばず、`AddResidueCountMonoidAct` を使う。

## 注意点

- `MOD > 0` が必要であり、違反はcompile時に検出する。
- `Count` は `Count{}` が0、`Count(1)` が1を表し、copy、`+=`、等値比較に
  対応する型とする。
- aggregateへ直接値を設定する場合、各 `count[r]` は非負の個数を表し、その和は
  区間長と一致しなければならない。
- 個数の加算結果は `Count` で表現できる必要がある。符号付き整数のoverflowは
  未定義動作になる。
- `composition(f, g)` は先に `g`、後に `f` を適用する。各作用は構築時と合成時に
  `[0, MOD)` へ正規化される。
- `op`、`mapping`、`e` の計算量は `Count` の各操作を $O(1)$ とした値である。
- 公開field `ResidueShift::shift` を直接変更して範囲外にした場合も、`mapping` と
  `composition` は使用時に再正規化する。通常は任意のdeltaをconstructorへ渡して
  作用を作る。
