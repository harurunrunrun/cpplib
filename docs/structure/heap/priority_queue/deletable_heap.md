---
title: Deletable Heap (削除可能ヒープ)
documentation_of: ../../../../src/structure/heap/priority_queue/deletable_heap.hpp
---

追加用のヒープと遅延削除用のヒープを組み合わせて、指定した値の削除を可能にした優先度付きキュー。
内部は `std::priority_queue<T, vector<T>, Compare>` なので、デフォルトの `std::less<T>` では最大値が先頭になる。

## API

```cpp
template<class T, class Compare = std::less<T>>
class DeletableHeap;

DeletableHeap()
explicit DeletableHeap(Compare compare)
bool empty() const
size_t size() const
void push(const T& value)
void push(T&& value)
void erase(const T& value)
const T& top()
void pop()
```

`erase(value)` は、現在格納されている要素のうち `value` と同値なもの 1 個を削除する。`top` は先頭の要素を返し、`pop` はそれを削除する。

## 時間計算量

$N$ をこれまでの `push` の回数とする。

- `empty`、`size`: $O(1)$
- `push`、`erase`、`pop`、`top`: 償却 $O(\log(N+1))$

## 空間計算量

まだ相殺されていない遅延削除を含め、$O(P)$（$P$ は `push` の回数）。

## 注意点

- 格納されていない値を `erase` してはならない（検査しない）。
- 空のヒープへの `top`、`pop`、`erase` は `std::out_of_range` を送出する。
- コピーはできない。
