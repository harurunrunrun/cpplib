---
title: Fibonacci Heap (フィボナッチヒープ)
documentation_of: ../../../../src/structure/heap/meldable/fibonacci_heap.hpp
---

キーの減少（decrease-key）と併合（meld）を高速に行う Fibonacci heap。`Compare(a, b)` が真のとき `a` を優先する（デフォルトの `std::less` では最小値が先頭）。

## API

```cpp
template<class T, class Compare = std::less<T>>
class FibonacciHeap;

FibonacciHeap()
explicit FibonacciHeap(Compare compare)
Handle push(const T& value)
Handle push(T&& value)
Handle emplace(Args&&... args)
void decrease_key(Handle handle, T value)
const T& top() const
void pop()
void meld(FibonacciHeap&& other)
bool empty() const
size_t size() const
```

`push`、`emplace` は追加した要素を指す `Handle` を返す。`Handle::valid()` は、その要素がまだヒープ内に残っているかを返す。
`decrease_key` は、`Compare` の順序でキーを改善する（優先度を上げる）場合だけ使える。
`meld` は `other` の全要素を取り込み、`other` を空にする。`other` の要素の `Handle` は、取り込んだ後もこのヒープで使える。

## 時間計算量

$N$ を要素数とする。

- `empty`、`size`、`top`: $O(1)$
- `push`、`emplace`、`decrease_key`: 償却 $O(1)$
- `pop`: 償却 $O(\log N)$
- `meld`: $O(1)$（根リストの連結と、ノードを保持する `std::list` の `splice`）

## 空間計算量

これまでに追加した要素数に比例する。削除済みのノードの領域も、`Handle` の検査のためにヒープの破棄まで保持する。

## 注意点

- 無効な `Handle` を渡した場合と、キーを悪化させる `decrease_key` は `std::invalid_argument` を送出する。空のヒープへの `top`、`pop` は `std::out_of_range` を送出する。
- 別のヒープ（取り込んでいないもの）の `Handle` を渡したかどうかは検査しない。
- コピーはできず、ムーブはできる。
