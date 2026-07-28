---
title: Skew Heap (スキューヒープ)
documentation_of: ../../../../src/structure/heap/meldable/skew_heap.hpp
---

meldのたび左右を交換して自己調整するmeldable heap。

## API

~~~cpp
SkewHeap<T, Compare>()
bool empty() const
size_t size() const
const T& top() const
void push(const T& value)
void push(T&& value)
void pop()
void meld(SkewHeap&& other)
~~~

## 時間計算量

- `empty`, `size`, `top`: $O(1)$
- `push`, `pop`, `meld`: amortized $O(\log(N+1))$

単一操作のworst-caseは $O(N)$、空間計算量は $O(N)$。

## 注意点

空heapの `top`, `pop` は `out_of_range`。copyはできずmoveできる。
