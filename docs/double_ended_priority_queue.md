---
title: Double-Ended Priority Queue (両端優先度付きキュー)
documentation_of: ../src/structure/other/double_ended_priority_queue.hpp
---

min-max heap による両端priority queue。重複する値を保持する。

# テンプレート引数

~~~cpp
DoubleEndedPriorityQueue<T, MAX_SIZE, Compare>
~~~

- 値の型 `T`
- 保持要素数の上限 `MAX_SIZE`
- 昇順を定める比較関数 `Compare`

# 構築

~~~cpp
DoubleEndedPriorityQueue(const Compare& compare = Compare())
DoubleEndedPriorityQueue(const vector<T>& values, const Compare& compare = Compare())
~~~

vector constructor は bottom-up に min-max heap を構築する。

# 操作

~~~cpp
int size() const
bool empty() const
void clear()
const T& min() const
const T& max() const
void push(const T& value)
T pop_min()
T pop_max()
~~~

空のqueueに対する取得・削除と、容量を超える追加は例外を送出する。

# 時間計算量

$N$ を現在の要素数とし、比較・swapを $O(1)$ とする。

- default constructor: 固定長arrayの初期化を含めて $O(\mathrm{MAX\_SIZE})$
- vector constructor: $O(\mathrm{MAX\_SIZE}+N)$
- `size`, `empty`, `clear`, `min`, `max`: $O(1)$
- `push`, `pop_min`, `pop_max`: worst-case $O(\log(N+1))$

# 空間計算量

- object全体: $O(\mathrm{MAX\_SIZE})$
- 各操作の追加領域: $O(1)$

## 注意点

- vector constructorは要素数が`MAX_SIZE`を超えると例外。
- `push`はsizeが`MAX_SIZE`に達している場合に例外。
- 空queueへの`min`, `max`, `pop_min`, `pop_max`は例外。
- `min`と`max`は保存要素へのconst参照、pop系は削除した値を返す。
- `clear`は$O(1)$で論理的に空にし、固定$O(MAX\_SIZE)$storageを再利用する。
