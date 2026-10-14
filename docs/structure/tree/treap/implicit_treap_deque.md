---
title: Implicit Treap Deque (暗黙トレープ両端キュー) [deque]
documentation_of: ../../../../src/structure/tree/treap/implicit_treap_deque.hpp
---

`ImplicitTreap` を両端キューとして使うための専用interface。

## 構築

```cpp
ImplicitTreapDeque<Monoid, MAX_SIZE> deque
```

空の列を作る。値型は `Monoid::S`。

- 時間計算量: $O(MAX\_SIZE)$
- 保存領域: $O(MAX\_SIZE)$

## 更新

```cpp
void push_front(const value_type& value)
void push_back(const value_type& value)
void pop_front()
void pop_back()
```

両端への追加・削除を行う。

- 時間計算量: 期待 $O(\log N)$
- 追加空間計算量: 期待 $O(\log N)$ の再帰stack

## query

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

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = ImplicitTreapDeque<Monoid>;
using Fixed = ImplicitTreapDeque<Monoid, N>;
```

可変長版は実行時の要素数・頂点数に応じて配列を確保する。ノードを追加する構造では伸長費用が償却 $O(1)$、既存の操作計算量を維持する。
領域は使用ノード数に比例し、木の距離索引など既存の段別領域は上記と同じ漸近量。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
