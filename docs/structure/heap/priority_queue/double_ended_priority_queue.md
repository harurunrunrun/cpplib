---
title: Double-Ended Priority Queue (両端優先度付きキュー) [double_ended_priority_queue]
documentation_of: ../../../../src/structure/heap/priority_queue/double_ended_priority_queue.hpp
---

min-max heap による両端優先度付きキュー。重複する値も保持する。

## テンプレート引数

```cpp
DoubleEndedPriorityQueue<T, MAX_SIZE = -1, Compare = std::less<T>>
```

- `T`: 値の型
- `MAX_SIZE`: 保持する要素数の上限。省略（`-1`）すると、実行時の要素数に合わせて `std::vector` を確保する可変長版になる
- `Compare`: 昇順を定める比較関数

```cpp
using Dynamic = DoubleEndedPriorityQueue<long long>;
```

## コンストラクタ

```cpp
explicit DoubleEndedPriorityQueue(const Compare& compare = Compare())
explicit DoubleEndedPriorityQueue(const vector<T>& values, const Compare& compare = Compare())
```

`vector` を受け取るコンストラクタは、ボトムアップに min-max heap を構築する。

## 操作

```cpp
int size() const
bool empty() const
void clear()
const T& min() const
const T& max() const
void push(const T& value)
T pop_min()
T pop_max()
```

`min`、`max` は保存されている最小・最大の要素への `const` 参照を返し、`pop_min`、`pop_max` は削除した値を返す。
可変長版で `T = bool` の場合、`min()`、`max()` は値で返す。

## 時間計算量

$N$ を現在の要素数とし、比較・交換を $O(1)$ とする。

- デフォルトコンストラクタ: 固定長版は配列の初期化を含めて $O(\texttt{MAX_SIZE})$、可変長版は $O(1)$
- `vector` を受け取るコンストラクタ: $O(N)$（固定長版はさらに $O(\texttt{MAX_SIZE})$）
- `size`、`empty`、`clear`、`min`、`max`: $O(1)$
- `push`: 最悪 $O(\log(N+1))$（可変長版では償却）
- `pop_min`、`pop_max`: 最悪 $O(\log(N+1))$

## 空間計算量

- 固定長版: オブジェクト全体で $O(\texttt{MAX_SIZE})$
- 可変長版: $O(N)$
- 各操作の追加領域: $O(1)$

## 注意点

- 固定長版で、`vector` を受け取るコンストラクタに `MAX_SIZE` を超える要素数を渡した場合と、要素数が `MAX_SIZE` に達している状態での `push` は `std::runtime_error` を送出する。
- 空のキューへの `min`、`max`、`pop_min`、`pop_max` は `std::runtime_error` を送出する。
- `clear` は $O(1)$ で論理的に空にし、確保済みの領域を再利用する。
- 可変長版では、`min`、`max` が返した参照は後続の再確保で無効になることがある。
