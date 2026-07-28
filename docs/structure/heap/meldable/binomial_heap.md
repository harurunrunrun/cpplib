---
title: Binomial Heap (二項ヒープ)
documentation_of: ../../../../src/structure/heap/meldable/binomial_heap.hpp
---

次数ごとに高々1本のbinomial treeを保持するmeld可能heap。

## API

~~~cpp
BinomialHeap<T, Compare>()
bool empty() const
size_t size() const
const T& top() const
void push(const T& value)
void push(T&& value)
void pop()
void meld(BinomialHeap&& other)
~~~

## 時間計算量

- `empty`, `size`: $O(1)$
- `top`, `push`, `pop`, `meld`: $O(\log(N+1))$

空間計算量は $O(N)$。

## 注意点

空heapの `top`, `pop` は `out_of_range`。copyはできずmoveできる。
