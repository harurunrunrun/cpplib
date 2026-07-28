---
title: Deletable Heap (削除可能ヒープ)
documentation_of: ../../../../src/structure/heap/priority_queue/deletable_heap.hpp
---

追加heapと遅延削除heapを組み合わせ、指定valueの削除を可能にするpriority queue。

## API

~~~cpp
DeletableHeap<T, Compare>()
bool empty() const
DeletableHeap<T, Compare>(Compare compare)
size_t size() const
void push(const T& value)
void push(T&& value)
void erase(const T& value)
const T& top()
void pop()
~~~

`erase(value)` は現在格納されている同値要素1個を削除する。

## 時間計算量

- `empty`, `size`: $O(1)$
- `push`, `erase`, `pop`: amortized $O(\log(N+1))$
- `top`: amortized $O(\log(N+1))$

空間計算量は、まだ相殺されていない遅延削除を含め $O(P)$ (`P` はpush回数)。

## 注意点

存在しないvalueを `erase` してはならない。空heapへの参照・削除は
`out_of_range`。
