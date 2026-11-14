---
title: Implicit Treap Deque (暗黙トレープ両端キュー) [deque]
documentation_of: ../../../../src/structure/tree/treap/implicit_treap_deque.hpp
---

`ImplicitTreap` を両端キューとして使うための専用インターフェースである。

## 構築

```cpp
ImplicitTreapDeque<Monoid, MAX_SIZE> deque
```

空の列を作る。値型は `Monoid` の `S` である。`MAX_SIZE` は省略でき、デフォルト値 `-1` では可変長版になる。

- 時間計算量: 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は $O(1)$
- 保存領域: 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は要素数に比例

## 更新

```cpp
void push_front(const value_type& value)
void push_back(const value_type& value)
void pop_front()
void pop_back()
```

両端への追加・削除を行う。

- 時間計算量: 期待 $O(\log N)$
- 追加空間計算量: 期待 $O(\log N)$ の再帰スタック

## 取得

```cpp
value_type get(int index)
int size() const
bool empty() const
```

`get` は 0-indexed で `index` 番目の要素を返し、期待 $O(\log N)$ である。`size` と `empty` は $O(1)$ である。

## 注意点

- 空の両端キューに対する `pop_front` と `pop_back` は `runtime_error` を送出する。
- `get` の添字は `[0, size())` でなければならない。固定長版で要素数が `MAX_SIZE` を超える追加も `runtime_error` を送出する。
- `Monoid` は元の `ImplicitTreap` と同じモノイドの要件を満たす必要がある。
- 容量を指定した固定配列版（`ImplicitTreapDeque<Monoid, N>`）と可変長版（`ImplicitTreapDeque<Monoid>`）は同じヘッダで利用できる。
