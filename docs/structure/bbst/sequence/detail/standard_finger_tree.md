---
title: Standard Finger Tree Implementation (標準Finger Tree内部実装)
documentation_of: ../../../../../src/structure/bbst/sequence/detail/standard_finger_tree.hpp
---

1〜4 個の要素からなる digit、Node2/Node3、再帰的な中間の木からなる、標準的な測度付き finger tree の内部実装である。通常は `finger_tree.hpp` の別名 `FingerTree` を利用する。

## API

~~~cpp
template<auto Monoid>
class StandardFingerTree;            // value_type = Monoid::S

StandardFingerTree()
bool empty() const
std::size_t size() const
void clear()
void push_front(const value_type& value)
void push_front(value_type&& value)
void push_back(const value_type& value)
void push_back(value_type&& value)
void pop_front()
void pop_back()
const value_type& front() const
const value_type& back() const
const value_type& at(std::size_t index) const
value_type fold() const
std::pair<StandardFingerTree, StandardFingerTree> split(std::size_t index) &&
std::pair<StandardFingerTree, StandardFingerTree> split_by(Predicate predicate) &&
void concatenate(StandardFingerTree&& other)
~~~

- 要素の型は `Monoid::S` で、各要素の測度は要素自身である。
- `fold`: 列順のモノイド積を返す。
- `split(i)`: 先頭 $i$ 要素と残りに分ける（$0\le i\le N$）。
- `split_by(predicate)`: 先頭からの積（接頭辞の測度）に対して `predicate` が初めて真になる要素を、右側の先頭にして分ける。単位元で既に真なら全体が右側、全体の積でも偽なら全体が左側になる。
- `split`, `split_by` は右辺値に対してのみ呼べる（元の木は消費される）。
- `concatenate(other)`: `other` を末尾に連結し、`other` を空にする。

## 時間計算量

連結する 2 つの木の大きさを $N$, $M$ とする。

- `empty`, `size`, `clear`, `front`, `back`, `fold`: $O(1)$
- `push_front`, `push_back`, `pop_front`, `pop_back`: 償却 $O(1)$
- `concatenate`: $O(\log(\min(N,M)+1))$
- `split`, `split_by`, `at`: $O(\log(N+1))$
- 空間計算量: $O(N)$。更新・分割・連結の一時領域は木の深さに比例して $O(\log(N+1))$

## 注意点

- `Monoid` の演算は結合的でなければならない。`split_by` の `predicate` は接頭辞の測度に対して単調（一度真になったら以降も真）でなければならない。
- 空の木の `front`/`back`/`pop_front`/`pop_back`、範囲外の `at`/`split` は `std::out_of_range` を投げる。
- コピーはできないが、ムーブはできる。
- 内部 API なので互換性は保証しない。
