---
title: Aggregate Deque (Monoid積付き両端キュー) [deque_operate_all_composite]
documentation_of: ../../../../src/structure/range_query/aggregation/aggregate_deque.hpp
---

`Monoid` が表す結合的な演算を使い、両端キュー内の要素を先頭から末尾の順に集約する。非可換なモノイドにも対応する。内部では 2 本のスタックを使う。

## `AggregateDeque`

```cpp
template<auto Monoid>
class AggregateDeque
```

`Monoid` は `S`、`op(S, S)`、`e()` を持つ constexpr オブジェクトとする。`value_type` は `Monoid::S` である。

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

`fold` は全要素のモノイド積を両端キューの順に計算して返し、空なら `Monoid.e()` を返す。`front` / `pop_front`（`back` / `pop_back`）の呼び出し時に対応する側のスタックが空であれば、論理的な順序を保ったまま要素をほぼ半分ずつ両側に再配置する。`capacity` は内部領域を予約するためのヒントであり、上限ではない。

## 時間計算量

要素数を $N$ とする。

- `size`, `empty`, `clear`, `fold`, `push_front`, `push_back`: $O(1)$
- `front`, `back`, `pop_front`, `pop_back`: 1 回あたりでは $O(N)$ の再配置が起こり得るが、操作列全体でならし $O(1)$

空間計算量は $O(N)$。

## 注意点

- 空の両端キューへの `front`、`back`、`pop_front`、`pop_back` は `std::runtime_error` を送出する。
- 返された参照は、次の非 const 操作で無効になる可能性がある。
