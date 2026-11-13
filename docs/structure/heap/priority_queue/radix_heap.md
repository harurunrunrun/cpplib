---
title: Radix Heap (基数ヒープ)
documentation_of: ../../../../src/structure/heap/priority_queue/radix_heap.hpp
---

取り出すキーが単調非減少であるような、符号なし整数キー用の優先度付きキュー（最小値を取り出す）。

## API

```cpp
template<class Key, class Value>
class RadixHeap;  // Key は符号なし整数型

RadixHeap()
bool empty() const
size_t size() const
Key last_popped_key() const
void push(Key key, const Value& value)
void push(Key key, Value&& value)
const pair<Key, Value>& top()
pair<Key, Value> pop()
```

`top` はキーが最小の要素を返し、`pop` はそれを削除して返す。`last_popped_key` は基準となるキー（最後に取り出した最小キー、初期値 $0$）を返す。
同じ最小キーを持つ値どうしの順序は規定しない。

## 時間計算量

$W$ を `numeric_limits<Key>::digits` とする。

- `empty`、`size`、`last_popped_key`、`push`: $O(1)$（`push` は償却）
- `top`、`pop`: 償却 $O(W)$
- $M$ 回の `push`/`pop` 全体: $O(MW)$

## 空間計算量

$O(N+W)$（$N$ は要素数）。

## 注意点

- `last_popped_key()` 未満のキーを `push` すると `std::invalid_argument` を送出する。
- 空のヒープへの `top`、`pop` は `std::out_of_range` を送出する。
