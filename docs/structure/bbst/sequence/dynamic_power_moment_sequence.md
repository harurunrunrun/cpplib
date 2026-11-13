---
title: Dynamic Power Moment Sequence (動的列べきモーメント) [GSS8]
documentation_of: ../../../../src/structure/bbst/sequence/dynamic_power_moment_sequence.hpp
---

挿入・削除・一点代入に対応し、区間内の位置で重み付けしたべきモーメントを返す列。内部では `ReversibleSplayTree` を使う。

## テンプレート引数

```cpp
DynamicPowerMomentSequence<T, MAX_POWER, MAX_SIZE = -1>
```

- `T`: 要素の型
- `MAX_POWER`: 扱うべき指数の上限（67 以下）
- `MAX_SIZE`: 同時に保持する要素数の上限。指定するとノード領域を固定確保し、省略（`-1`）すると `std::vector` で必要に応じて確保する可変長版になる

## API

### `DynamicPowerMomentSequence(values)`

初期列 `values`（`std::vector<T>`）から構築する。

### `size()` / `empty()`

現在の要素数と、空かどうかを返す。

### `insert(position, value)` / `erase(position)` / `set(position, value)`

位置 `position` の直前への挿入、位置 `position` の削除、位置 `position` への代入を行う。

### `power_moment(left, right, power)`

半開区間 $[\mathrm{left},\mathrm{right})$ の要素を順に $a_1,\ldots,a_L$ としたとき、$\sum_{i=1}^{L} i^{p}a_i$（$p$ は `power`）を返す。

## 時間計算量

要素数を $N$、$P=\texttt{MAX_POWER}$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(NP^2)$（固定長版はさらに $O(\texttt{MAX_SIZE}\,P)$ の初期化） | 固定長版 $O(\texttt{MAX_SIZE}\,P)$、可変長版 $O(NP)$ |
| `size()` / `empty()` | $O(1)$ | $O(1)$ |
| `insert` / `erase` / `set` | 償却 $O(P^2\log N)$ | $O(1)$ |
| `power_moment` | 償却 $O(P^2\log N)$ | $O(1)$ |

## 注意点

- `power <= MAX_POWER <= 67` でなければならない。`MAX_POWER > 67` は `static_assert`、`power > MAX_POWER` は `std::runtime_error` になる。
- 固定長版では、同時に保持する要素数が `MAX_SIZE` 以下でなければならない。
- 演算は `T` の算術で行う。符号なし整数型なら自然にその法での計算になる。
- 添字・容量の違反では `std::runtime_error` を投げる。
