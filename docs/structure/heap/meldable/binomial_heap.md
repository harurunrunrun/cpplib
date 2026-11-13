---
title: Binomial Heap (二項ヒープ)
documentation_of: ../../../../src/structure/heap/meldable/binomial_heap.hpp
---

次数ごとに高々 1 本の二項木を保持する、併合（meld）可能なヒープ。`Compare(a, b)` が真のとき `a` を優先する（デフォルトの `std::less` では最小値が先頭）。

## API

```cpp
template<class T, class Compare = std::less<T>>
class BinomialHeap;

BinomialHeap()
explicit BinomialHeap(Compare compare)
bool empty() const
size_t size() const
const T& top() const
void push(const T& value)
void push(T&& value)
void pop()
void meld(BinomialHeap&& other)
```

`top` は最も優先される要素を返し、`pop` はそれを削除する。`meld` は `other` の全要素を取り込み、`other` を空にする。

## 時間計算量

$N$ を要素数とする。

- `empty`、`size`: $O(1)$
- `top`、`push`、`pop`、`meld`: $O(\log(N+1))$（`top` は根を走査するので $O(1)$ ではない）

## 空間計算量

$O(N)$。

## 注意点

- 空のヒープへの `top`、`pop` は `std::out_of_range` を送出する。
- コピーはできず、ムーブはできる。
