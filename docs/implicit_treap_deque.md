---
title: Implicit Treap Deque (暗黙トレープ両端キュー) [deque]
documentation_of: ../src/structure/tree/implicit_treap_deque.hpp
---

`ImplicitTreap` を両端キューとして使うための専用interface。

# 構築

```cpp
ImplicitTreapDeque<Monoid, MAX_SIZE> deque
```

空の列を作る。値型は `Monoid::S`。

- 時間計算量: $O(MAX\_SIZE)$
- 保存領域: $O(MAX\_SIZE)$

# 更新

```cpp
void push_front(const value_type& value)
void push_back(const value_type& value)
void pop_front()
void pop_back()
```

両端への追加・削除を行う。

- 時間計算量: 期待 $O(\log N)$
- 追加空間計算量: 期待 $O(\log N)$ の再帰stack

# query

```cpp
value_type get(int index)
int size() const
bool empty() const
```

`get` は0-indexedの要素を返し、期待 $O(\log N)$。`size` と `empty` は $O(1)$。

## 注意点

- 空のdequeに対する `pop_front` と `pop_back` は `runtime_error`。
- `get` の添字は `[0,size())`。要素数が `MAX_SIZE` を超える追加も `runtime_error`。
- `Monoid` は元の `ImplicitTreap` と同じモノイド要件を満たす。
