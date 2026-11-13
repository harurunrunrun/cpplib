---
title: Pairing Heap (ペアリングヒープ)
documentation_of: ../../../../src/structure/heap/meldable/pairing_heap.hpp
---

2-pass pairing を用いる、併合（meld）可能な優先度付きキュー。`Compare(a, b)` が真のとき `a` を優先する（デフォルトの `std::less` では最小値が先頭）。

## API

```cpp
template<class T, class Compare = std::less<T>>
class PairingHeap;

PairingHeap()
explicit PairingHeap(Compare compare)
bool empty() const
size_t size() const
const T& top() const
void push(const T& value)
void push(T&& value)
void pop()
void meld(PairingHeap&& other)
void clear()
```

`top` は最も優先される要素を返し、`pop` はそれを削除する。`meld` は `other` の全要素を取り込み、`other` を空にする。

## 時間計算量

$N$ を要素数とする。

- `empty`、`size`、`top`: $O(1)$
- `push`、`meld`: $O(1)$
- `pop`: 償却 $O(\log N)$
- `clear`: 全ノードを解放するので $O(N)$

## 空間計算量

$O(N)$。

## 注意点

- 空のヒープへの `top`、`pop` は `std::out_of_range` を送出する。
- コピーはできず、ムーブはできる。
- `Compare` は、要素を格納している間に順序を変えてはならない。
