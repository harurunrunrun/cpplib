---
title: Aggregate Deque (Monoid積付き両端キュー) [deque_operate_all_composite]
documentation_of: ../../../../src/structure/range_query/aggregation/aggregate_deque.hpp
---

`Monoid` が表す結合則付き演算を、deque内の先頭から末尾の順で集約する。
非可換Monoidにも対応する。

## `AggregateDeque`

```cpp
template<auto Monoid>
class AggregateDeque
```

`Monoid` は `S`、`op(S,S)`、`e()` を持つ
`Monoid` 型のconstexpr objectとする。`value_type` は `Monoid::S`。

```cpp
AggregateDeque()
explicit AggregateDeque(std::size_t capacity)
std::size_t size() const
bool empty() const
void clear()
void push_front(const value_type& value)
void push_back(const value_type& value)
const value_type& front()
const value_type& back()
void pop_front()
void pop_back()
value_type fold() const
```

`fold` は全要素のMonoid積をdeque順で返し、空なら `Monoid.e()` を返す。
片側が空になったときは、論理順を保ったまま要素をほぼ半分ずつに再配置する。
`capacity` は内部領域の予約ヒントであり、上限ではない。

## 時間計算量

- `size`, `empty`, `clear`, `fold`, `push_front`, `push_back`: $O(1)$
- `front`, `back`, `pop_front`, `pop_back`: 1回では $O(N)$ の再配置があり、操作列全体で償却 $O(1)$

空間計算量は $O(N)$。

## 注意点

空dequeへの `front`、`back`、`pop_front`、`pop_back` は `runtime_error`。
返された参照は、次の非const操作で無効になる可能性がある。
