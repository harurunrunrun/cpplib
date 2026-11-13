---
title: Skew Heap (スキューヒープ)
documentation_of: ../../../../src/structure/heap/meldable/skew_heap.hpp
---

併合（meld）のたびに左右の子を交換して自己調整する、併合可能なヒープ。`Compare(a, b)` が真のとき `a` を優先する（デフォルトの `std::less` では最小値が先頭）。

## API

```cpp
template<class T, class Compare = std::less<T>>
class SkewHeap;

SkewHeap()
explicit SkewHeap(Compare compare)
bool empty() const
size_t size() const
const T& top() const
void push(const T& value)
void push(T&& value)
void pop()
void meld(SkewHeap&& other)
```

`top` は最も優先される要素を返し、`pop` はそれを削除する。`meld` は `other` の全要素を取り込み、`other` を空にする。

## 時間計算量

$N$ を要素数とする。

- `empty`、`size`、`top`: $O(1)$
- `push`、`pop`、`meld`: 償却 $O(\log(N+1))$。1 回の操作の最悪は $O(N)$

## 空間計算量

$O(N)$。併合は再帰で行うため、1 回の操作の最悪の再帰の深さは $O(N)$ である。

## 注意点

- 空のヒープへの `top`、`pop` は `std::out_of_range` を送出する。
- コピーはできず、ムーブはできる。
