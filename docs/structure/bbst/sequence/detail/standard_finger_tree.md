---
title: Standard Finger Tree Implementation (標準Finger Tree内部実装)
documentation_of: ../../../../../src/structure/bbst/sequence/detail/standard_finger_tree.hpp
---

digit 1–4、Node2/Node3、中間Finger Treeからなる標準的なmeasured finger
treeの内部実装。通常は `finger_tree.hpp` の `FingerTree` aliasを利用する。

## API

~~~cpp
StandardFingerTree<Monoid>()
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
pair<StandardFingerTree, StandardFingerTree> split(size_t index) &&
pair<StandardFingerTree, StandardFingerTree> split_by(Predicate predicate) &&
void concatenate(StandardFingerTree&& other)
~~~

`split_by(predicate)` は、prefix measureに対して `predicate` が初めて真になる
要素をright側の先頭にする。

## 時間計算量

- `empty`, `size`, `clear`, `front`, `back`, `fold`: $O(1)$
- `push_front`, `push_back`, `pop_front`, `pop_back`: amortized $O(1)$
- `concatenate`: $O(\log(\min(N,M)+1))$
- `split`, `split_by`, `at`: $O(\log(N+1))$

空間計算量は $O(N)$。更新・split・concatenateの一時領域は木のdepthに比例
して $O(\log(N+1))$。

## 注意点

`Monoid` は結合的で、`split_by` のpredicateはprefix measureに対して単調で
あること。空treeのfront/back/popと範囲外indexは `out_of_range`。内部APIの
互換性は保証しない。
