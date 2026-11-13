---
title: Modulo Multiset Counter (剰余変換付き多重集合カウンタ)
documentation_of: ../../../../src/structure/ordered_set/transform_multiset/modulo_multiset_counter.hpp
---

非負整数をキーとする個数を保持し、すべてのキー $x$ を一括で $x \gets x \bmod m$ に変換する。初期状態として $1, 2, \ldots, N$ を各 1 個持たせられる。この密な初期部分は区間ごとに一定な関数の差分を treap に保持するので、$N$ 個を列挙しない。後から `add` した疎なキーは `std::map` で保持する。

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

$1 \le x \le N$（$N$ = `initial_maximum`）の各キー $x$ の個数を 1 に設定する。`initial_maximum` が 0 なら空の状態になる。

- 時間計算量: $O(1)$
- 空間計算量: $O(1)$

コピーはできない（削除されている）。ムーブは可能である。

## `apply_mod`

```cpp
void apply_mod(Key modulus)
```

保持する各キー $x$ を $x \bmod m$（$m$ = `modulus`）へ移し、移動先が同じキーの個数を合算する。`modulus == 0` なら `std::runtime_error` を送出する。

変換対象となる密な表現の差分ノード数を $D$、`modulus` 以上の疎なキーの数を $E$、変換後の差分ノードの総数を $D'$ とする。

- 時間計算量: $O(D\log(D'+1)+E\log(E+1))$
- 追加空間計算量: $O(D)$

特に、現在のすべてのキーが `modulus` 未満なら $O(\log(D+1))$ である。

## `add`

```cpp
void add(Key value, Count delta)
```

`value` の個数に `delta` を加える。個数が負になる操作も許す。$E$ は疎なキーの数である。

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

`value` の個数を `long long` で返す。`long long` の範囲外なら `std::overflow_error` を送出する。

- 時間計算量: $O(\log(D+1)+\log(E+1))$
- 追加空間計算量: $O(1)$

## `explicit_key_count`

```cpp
std::size_t explicit_key_count() const
```

`add` 由来の疎な表現に現在残っているキーの数を返す。密な初期部分のキーの数は含まない。

- 時間計算量: $O(1)$

## 注意点

- `add` の引数は `long long` だが、同じキーへの累積値と密な表現の差分係数は `__int128_t` で保持する。`count_wide` の結果も `__int128_t` である。
- オブジェクト全体の保存領域は $O(D+E)$ である。
- 疎な表現で、加算の結果個数が 0 になったキーは削除される。
