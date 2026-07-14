---
title: Dynamic Power Moment Sequence (動的列べきモーメント) [GSS8]
documentation_of: ../src/structure/bbst/dynamic_power_moment_sequence.hpp
---

挿入・削除・一点代入に対応し、区間の位置重み付きべきモーメントを返す列です。

## API

### `DynamicPowerMomentSequence(values)`

初期列から構築します。

### `size()` / `empty()`

現在の要素数、空かどうかを返します。

### `insert(position, value)` / `erase(position)` / `set(position, value)`

指定位置への挿入、削除、代入を行います。

### `power_moment(left, right, power)`

半開区間の要素を $a_1,\ldots,a_L$ としたとき、$\sum_{i=1}^{L} i^{power}a_i$ を返します。

## API別の時間計算量・空間計算量

$P=MAX\_POWER$ とします。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(NP^2)$ | $O(MAX\_SIZE\,P)$ |
| `size()` / `empty()` | $O(1)$ | $O(1)$ |
| `insert` / `erase` / `set` | 償却 $O(P^2\log N)$ | $O(1)$ |
| `power_moment` | 償却 $O(P^2\log N)$ | $O(1)$ |

## 注意点

- `power <= MAX_POWER <= 67` が必要です。
- 同時に保持する要素数は `MAX_SIZE` 以下でなければなりません。
- 演算は `T` の算術で行い、符号なし整数では自然にその法で計算されます。
- 添字・容量・べき次数違反では `runtime_error` を送出します。
