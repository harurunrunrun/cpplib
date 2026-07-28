---
title: Aggregate Queue (Monoid積付きキュー) [queue_operate_all_composite]
documentation_of: ../../../../src/structure/range_query/aggregation/aggregate_queue.hpp
---

`Monoid` が表す結合則付き演算を、queue内の先頭から末尾の順で集約する。
非可換Monoidにも対応する。

## `AggregateQueue`

```cpp
template<auto Monoid>
class AggregateQueue
```

`Monoid` は `S`、`op(S,S)`、`e()` を持つ
`Monoid` 型のconstexpr objectとする。`value_type` は `Monoid::S`。

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

`push` は末尾追加、`front` は先頭参照、`pop` は先頭削除を行う。
`fold` は全要素のMonoid積をqueue順で返し、空なら `Monoid.e()` を返す。
`capacity` は2本の内部stackへ領域を予約するヒントであり、上限ではない。

## 時間計算量

- `size`, `empty`, `clear`, `fold`: $O(1)$
- `push`, `front`, `pop`: 1回では $O(N)$ の移し替えがあり、操作列全体で償却 $O(1)$

空間計算量は $O(N)$。

## 注意点

空queueへの `front` と `pop` は `runtime_error`。返された参照は、次の非const操作で
無効になる可能性がある。
