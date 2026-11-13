---
title: Aggregate Queue (Monoid積付きキュー) [queue_operate_all_composite]
documentation_of: ../../../../src/structure/range_query/aggregation/aggregate_queue.hpp
---

`Monoid` が表す結合的な演算を使い、キュー内の要素を先頭から末尾の順に集約する。非可換なモノイドにも対応する。内部では 2 本のスタックを使う。

## `AggregateQueue`

```cpp
template<auto Monoid>
class AggregateQueue
```

`Monoid` は `S`、`op(S, S)`、`e()` を持つ constexpr オブジェクトとする。`value_type` は `Monoid::S` である。

```cpp
AggregateQueue()
explicit AggregateQueue(std::size_t capacity)
std::size_t size() const
bool empty() const
void clear()
void push(const value_type& value)
void push(value_type&& value)
const value_type& front()
void pop()
value_type fold() const
```

`push` は末尾への追加、`front` は先頭の参照、`pop` は先頭の削除を行う。`fold` は全要素のモノイド積をキューの順に計算して返し、空なら `Monoid.e()` を返す。`capacity` は 2 本の内部スタックの領域を予約するためのヒントであり、上限ではない。

## 時間計算量

要素数を $N$ とする。

- `size`, `empty`, `clear`, `fold`, `push`: $O(1)$
- `front`, `pop`: 1 回あたりでは $O(N)$ の移し替えが起こり得るが、操作列全体でならし $O(1)$

空間計算量は $O(N)$。

## 注意点

- 空のキューへの `front` と `pop` は `std::runtime_error` を送出する。
- 返された参照は、次の非 const 操作で無効になる可能性がある。
