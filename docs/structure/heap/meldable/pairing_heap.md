---
title: Pairing Heap (ペアリングヒープ)
documentation_of: ../../../../src/structure/heap/meldable/pairing_heap.hpp
---

2-pass pairingを用いるmeld可能な優先度付きqueue。`Compare(a,b)` が真の要素を優先する。

## API

~~~cpp
PairingHeap<T, Compare>()
PairingHeap(Compare compare)
bool empty() const
size_t size() const
const T& top() const
void push(const T& value)
void push(T&& value)
void pop()
void meld(PairingHeap&& other)
void clear()
~~~

`top`, `pop` は空なら `out_of_range`。`meld` 後の `other` は空になる。

## 時間計算量

- `empty`, `size`, `top`, `clear`: $O(1)$
- `push`, `meld`: $O(1)$
- `pop`: amortized $O(\log N)$

空間計算量は $O(N)$。

## 注意点

copyはできずmoveできる。`Compare` は格納中に順序を変えないこと。
