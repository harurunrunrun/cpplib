---
title: Fibonacci Heap (フィボナッチヒープ)
documentation_of: ../../../../src/structure/heap/meldable/fibonacci_heap.hpp
---

decrease-keyとmeldを高速に行うFibonacci heap。

## API

~~~cpp
FibonacciHeap<T, Compare>()
Handle push(const T& value)
Handle push(T&& value)
Handle emplace(Args&&... args)
void decrease_key(Handle handle, T value)
const T& top() const
void pop()
void meld(FibonacciHeap&& other)
bool empty() const
size_t size() const
~~~

`Handle::valid()` は要素がheap内に残っているかを返す。`decrease_key` は
`Compare` の順序でkeyを改善する場合だけ使用できる。

## 時間計算量

- `empty`, `size`, `top`: $O(1)$
- `push`, `emplace`, `meld`: amortized $O(1)$
- `decrease_key`: amortized $O(1)$
- `pop`: amortized $O(\log N)$

空間計算量は $O(N)$。削除済みnodeの領域はheapの破棄までhandle検査用に保持する。

## 注意点

無効handleまたはkey増加は `invalid_argument`、空heapの参照・削除は
`out_of_range`。copyはできずmoveできる。
