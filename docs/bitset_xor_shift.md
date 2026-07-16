---
title: Bitset XOR Shift (ビット集合XORシフト)
documentation_of: ../src/structure/other/bitset_xor_shift.hpp
---

固定最大bit数のbit列を64-bit word列で保持し、自己参照する `bits ^= bits << k` と `bits ^= bits >> k` を追加領域なしで行う。

```cpp
template<size_t MAX_BITS>
class BitsetXorShift;
```

## Constructor

```cpp
explicit BitsetXorShift(size_t size = MAX_BITS);
```

有効bit数を `size` とする零初期化済みbit列を作る。

## `capacity`, `size`, `empty`

```cpp
static constexpr size_t capacity() noexcept;
size_t size() const noexcept;
bool empty() const noexcept;
```

compile時最大bit数、有効bit数、有効bit数が0かを返す。

## `test`, `operator[]`

```cpp
bool test(size_t position) const;
bool operator[](size_t position) const;
```

0-indexed bit `position` の値を返す。

## `set`, `reset`, `flip`

```cpp
BitsetXorShift& set(size_t position, bool value = true);
BitsetXorShift& reset(size_t position);
BitsetXorShift& flip(size_t position);

BitsetXorShift& set();
BitsetXorShift& reset() noexcept;
BitsetXorShift& flip();
```

引数付きAPIは1 bit、引数なしAPIは全有効bitを変更する。最後のwordにある有効範囲外のbitは常に0へ保つ。

## `count`, `any`, `none`, `all`

```cpp
size_t count() const noexcept;
bool any() const noexcept;
bool none() const noexcept;
bool all() const noexcept;
```

立っているbit数、1が存在するか、全て0か、全て1かを返す。size 0では `none()` と `all()` がともに真である。

## `xor_left_shift`

```cpp
BitsetXorShift& xor_left_shift(size_t shift) noexcept;
```

変更前のbit列を `old` として `bits = old ^ (old << shift)` を行う。有効範囲から左へ出たbitは捨てる。in-place更新は上位wordから下位wordの順に行うため、未読のsource wordを上書きしない。

## `xor_right_shift`

```cpp
BitsetXorShift& xor_right_shift(size_t shift) noexcept;
```

変更前のbit列を `old` として `bits = old ^ (old >> shift)` を行う。in-place更新は下位wordから上位wordの順に行う。

`shift == 0` では全bitが0になる。`shift >= size()` ではshift項が0なのでbit列を変更しない。

## XOR・比較

```cpp
BitsetXorShift& operator^=(const BitsetXorShift& other);
friend BitsetXorShift operator^(BitsetXorShift left, const BitsetXorShift& right);
friend bool operator==(const BitsetXorShift& left, const BitsetXorShift& right);
friend bool operator!=(const BitsetXorShift& left, const BitsetXorShift& right);
```

同じ有効bit数のbit列同士でXORを取る。比較では有効bit数も比較する。

## 時間計算量

$W=\lceil\mathrm{size}/64\rceil$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| constructor, `capacity`, `size`, `empty`, 引数付き `set`, `reset`, `flip`, `test`, `operator[]` | $O(1)$ | $O(1)$ |
| 引数なし `set`, `reset`, `flip`, `count`, `any`, `none`, `all` | $O(W)$ | $O(1)$ |
| `xor_left_shift`, `xor_right_shift` | $O(W)$、ただし `shift >= size()` は $O(1)$ | $O(1)$ |
| XOR・比較 | $O(W)$ | `operator^` の返り値を除き $O(1)$ |

object自体は $O(\lceil\mathrm{MAX\_BITS}/64\rceil)$ wordを持つ。

## 注意点

- constructorの `size` が `MAX_BITS` を超えると `runtime_error` を送出する。
- bit位置が `size()` 以上なら、位置を受け取るAPIは `runtime_error` を送出する。
- 異なる有効bit数のobjectに `operator^=` または `operator^` を使うと `runtime_error` を送出する。
- shiftは通常の左辺更新と同じく、各右辺bitを変更前のbit列から読む。
