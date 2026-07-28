---
title: Radix Heap (基数ヒープ)
documentation_of: ../../../../src/structure/heap/priority_queue/radix_heap.hpp
---

取り出すkeyが単調非減少であるunsigned integer用priority queue。

## API

~~~cpp
RadixHeap<Key, Value>()
bool empty() const
size_t size() const
Key last_popped_key() const
void push(Key key, const Value& value)
void push(Key key, Value&& value)
const pair<Key, Value>& top()
pair<Key, Value> pop()
~~~

同じ最小keyを持つvalue間の順序は規定しない。

## 時間計算量

`W = numeric_limits<Key>::digits` とする。

- `empty`, `size`, `last_popped_key`, `push`: $O(1)$
- `top`, `pop`: amortized $O(W)$
- $M$ 回のpush/pop全体: $O(MW)$

空間計算量は $O(N+W)$。

## 注意点

最後にpopしたkey未満をpushすると `invalid_argument`。空heapへの
`top`, `pop` は `out_of_range`。
