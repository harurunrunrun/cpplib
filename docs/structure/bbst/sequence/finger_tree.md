---
title: Finger Tree (フィンガー列木)
documentation_of: ../../../../src/structure/bbst/sequence/finger_tree.hpp
---

A standard measured finger tree using digits of one to four items, Node2/Node3, and a recursive middle tree.

## API

~~~cpp
FingerTree<Monoid>()
bool empty() const
size_t size() const
void clear()
void push_front(const value_type& value)
void push_front(value_type&& value)
void push_back(const value_type& value)
void push_back(value_type&& value)
void pop_front()
void pop_back()
const value_type& front() const
const value_type& back() const
const value_type& at(size_t index) const
value_type fold() const
pair<FingerTree, FingerTree> split(size_t index) &&
pair<FingerTree, FingerTree> split_by(Predicate predicate) &&
void concatenate(FingerTree&& other)
~~~

`fold` は列順にmonoid積を返す。`split(i)` はprefix `i` 要素とsuffixへ分ける。
`split_by(predicate)` places the first element making the prefix predicate true at the beginning of the right tree.

## 時間計算量

- `empty`, `size`, `clear`, `fold`, `front`, `back`: $O(1)$
- `push_front`, `push_back`, `pop_front`, `pop_back`: amortized $O(1)$
- `concatenate`: $O(\log(\min(N,M)+1))$
- `split`, `split_by`, `at`: $O(\log(N+1))$

空間計算量は $O(N)$。

## 注意点

`split_by` requires a predicate monotone over prefix measures.
copy不可・move可能。範囲外indexや空列のpop/referenceは `out_of_range`。
