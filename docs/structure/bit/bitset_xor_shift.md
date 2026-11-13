---
title: Bitset XOR Shift (ビット集合XORシフト)
documentation_of: ../../../src/structure/bit/bitset_xor_shift.hpp
---

ビット列を 64 ビット のワード列で保持し、自己参照する `bits ^= bits << k` と `bits ^= bits >> k` を追加領域なしで行う。

```cpp
template<std::size_t MAX_BITS = std::numeric_limits<std::size_t>::max()>
class BitsetXorShift;
```

`MAX_BITS` を指定すると、最大ビット数 `MAX_BITS` 分のワードを `std::array` で固定確保する。省略すると、実行時に指定したビット数に合わせて `std::vector` を確保する可変長版になる。

## コンストラクタ

```cpp
explicit BitsetXorShift(std::size_t size = MAX_BITS);   // 可変長版のデフォルトは 0
```

有効ビット数を `size` とする、0 で初期化されたビット列を作る。

## `capacity`, `size`, `empty`

```cpp
static constexpr std::size_t capacity() noexcept;   // 可変長版は非 static で size() と同じ値
std::size_t size() const noexcept;
bool empty() const noexcept;
```

最大ビット数（固定長版は `MAX_BITS`、可変長版は有効ビット数）、有効ビット数、有効ビット数が 0 かどうかを返す。

## `test`, `operator[]`

```cpp
bool test(std::size_t position) const;
bool operator[](std::size_t position) const;
```

0-indexed bit `position` の値を返す。

## `set`, `reset`, `flip`

```cpp
BitsetXorShift& set(std::size_t position, bool value = true);
BitsetXorShift& reset(std::size_t position);
BitsetXorShift& flip(std::size_t position);

BitsetXorShift& set();
BitsetXorShift& reset() noexcept;
BitsetXorShift& flip();
```

引数付きの API は 1 ビット、引数なし API は全有効ビット を変更する。最後のワードにある有効範囲外のビット は常に 0 へ保つ。

## `count`, `any`, `none`, `all`

```cpp
std::size_t count() const noexcept;
bool any() const noexcept;
bool none() const noexcept;
bool all() const noexcept;
```

立っているビット数、1 が存在するか、全て 0 か、全て 1 かを返す。有効ビット数が 0 のときは `none()` と `all()` がともに真である。

## `xor_left_shift`

```cpp
BitsetXorShift& xor_left_shift(std::size_t shift) noexcept;
```

変更前の ビット列を `old` として `bits = old ^ (old << shift)` を行う。有効範囲から左へ出たビット は捨てる。インプレースの更新は上位ワードから下位ワードの順に行うため、まだ読んでいない元のワード を上書きしない。

## `xor_right_shift`

```cpp
BitsetXorShift& xor_right_shift(std::size_t shift) noexcept;
```

変更前の ビット列を `old` として `bits = old ^ (old >> shift)` を行う。インプレースの更新は下位ワードから上位ワードの順に行う。

`shift == 0` では全ビットが 0 になる。`shift >= size()` では シフト項が 0 なので ビット列を変更しない。

## XOR ・比較

```cpp
BitsetXorShift& operator^=(const BitsetXorShift& other);
friend BitsetXorShift operator^(BitsetXorShift left, const BitsetXorShift& right);
friend bool operator==(const BitsetXorShift& left, const BitsetXorShift& right);
friend bool operator!=(const BitsetXorShift& left, const BitsetXorShift& right);
```

同じ有効ビット数の ビット列同士で XOR を取る。比較では有効ビット数も比較する。

## 時間計算量

$W=\lceil\texttt{size}/64\rceil$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| コンストラクタ | 固定長版 $O(\lceil\texttt{MAX_BITS}/64\rceil)$、可変長版 $O(W)$ | 本体の保存領域のみ |
| `capacity`, `size`, `empty`, 引数付き `set`, `reset`, `flip`, `test`, `operator[]` | $O(1)$ | $O(1)$ |
| 引数なし `set`, `reset`, `flip`, `count`, `any`, `none`, `all` | $O(W)$ | $O(1)$ |
| `xor_left_shift`, `xor_right_shift` | $O(W)$、ただし `shift >= size()` は $O(1)$ | $O(1)$ |
| XOR・比較 | $O(W)$ | `operator^` の戻り値を除き $O(1)$ |

オブジェクト自体は、固定長版では $\lceil\texttt{MAX_BITS}/64\rceil$ ワード、可変長版では $W$ ワードを持つ。

## 注意点

- コンストラクタの `size` が `MAX_BITS` を超えると `std::runtime_error` を投げる。
- ビット位置が `size()` 以上なら、位置を受け取る API は `std::runtime_error` を投げる。
- 有効ビット数が異なるオブジェクト同士に `operator^=` または `operator^` を使うと `std::runtime_error` を投げる。
- シフトは通常の代入と同じく、右辺の各ビットを変更前のビット列から読む。
