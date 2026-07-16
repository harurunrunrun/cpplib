---
title: Modulo Multiset Counter (剰余変換付き多重集合カウンタ)
documentation_of: ../src/structure/other/modulo_multiset_counter.hpp
---

非負整数を key とする個数を保持し、全 key を一括で $x\gets x\bmod m$ に
変換する。初期状態として $1,2,\ldots,N$ を各1個持たせられる。この密な初期部分は
区間一定関数の差分をtreapに保持するため、$N$ 個を列挙しない。後から `add` した
疎な key は `std::map` で保持する。

## 型

```cpp
struct ModuloMultisetCounter;
using ModCounter = ModuloMultisetCounter;
```

- `Key = std::uint64_t`
- `Count = long long`
- `WideCount = __int128_t`

`ModCounter` は互換用の別名である。

## 構築

```cpp
explicit ModuloMultisetCounter(Key initial_maximum = 0)
```

$1\le x\le initial\_maximum$ の各 key に個数1を設定する。0なら空。

- 時間計算量: $O(\log 2)=O(1)$
- 空間計算量: $O(1)$

copyはできない。moveは可能。

## `apply_mod`

```cpp
void apply_mod(Key modulus)
```

保持する各 key $x$ を $x\bmod modulus$ へ移し、同じ剰余の個数を加算する。
`modulus == 0` なら `runtime_error`。

変換対象となる密表現の差分node数を $D$、`modulus` 以上の疎なkey数を $E$、
変換後の差分node総数を $D'$ とする。

- 時間計算量: $O(D\log(D'+1)+E\log(E+1))$
- 追加空間計算量: $O(D)$

特に、現在のすべてのkeyが `modulus` 未満なら $O(\log(D+1))$。

## `add`

```cpp
void add(Key value, Count delta)
```

`value` の個数に `delta` を加える。個数を負にする操作も許す。

- 時間計算量: $O(\log(E+1))$
- 追加空間計算量: $O(1)$

## `count_wide`

```cpp
WideCount count_wide(Key value) const
```

`value` の現在の個数を `__int128_t` で返す。

- 時間計算量: $O(\log(D+1)+\log(E+1))$
- 追加空間計算量: $O(1)$

## `count`

```cpp
Count count(Key value) const
```

`value` の個数を `long long` で返す。範囲外なら `overflow_error`。

- 時間計算量: $O(\log(D+1)+\log(E+1))$
- 追加空間計算量: $O(1)$

## `explicit_key_count`

```cpp
std::size_t explicit_key_count() const
```

`add` 由来の疎な表現に現在残るkey数を返す。密な初期部分のkey数は含まない。

- 時間計算量: $O(1)$

## 注意点

`add` の引数は `long long` だが、同じkeyへの累積値と密表現の差分係数は `__int128_t` で保持する。
`count_wide` の最終結果も `__int128_t`。object全体の保存領域は
$O(D+E)$。同じkeyへの加算結果が0になった疎nodeは削除される。

