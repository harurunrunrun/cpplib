---
title: Rollback Array
documentation_of: ../src/structure/array/rollback_array.hpp
---

1点更新を取り消せる配列。

# テンプレート引数

```cpp
RollbackArray<T, MAX_SIZE, MAX_UPDATE>
```

- 要素の型 `T`
- 配列長の上限 `MAX_SIZE`
- 同時に保持する更新履歴数の上限 `MAX_UPDATE`

オブジェクト自体のcopy/moveはできない。

# コンストラクタ

```cpp
(1) RollbackArray(int n = MAX_SIZE)
(2) RollbackArray(int n, const T& value)
(3) RollbackArray(const vector<T>& v)
(4) RollbackArray(const array<T, N>& v)
```

1. 長さ `n`、初期値 `T{}` で初期化する。
2. 長さ `n`、初期値 `value` で初期化する。
3. `v` で初期化する。
4. `v` で初期化する。

## 時間計算量

- $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_UPDATE})$

固定長の初期値・更新値・履歴領域を構築した後、使用する $n$ 要素を初期化する。

# set

```cpp
void set(int k, const T& x)
```

`k` 番目を `x` に変更し、変更前の値を履歴に追加する。

## 時間計算量

- $O(1)$

# get

```cpp
const T& get(int k) const
```

`k` 番目を返す。

## 時間計算量

- $O(1)$

# snapshot

```cpp
using Snapshot = int;

Snapshot snapshot() const
```

現在の更新履歴数を返す。

`rollback` で取り消した履歴上のsnapshotは無効になる。

## 時間計算量

- $O(1)$

# rollback

```cpp
void rollback(Snapshot s)
```

`snapshot()` が `s` を返した時点まで更新を戻す。

現在より後のsnapshot、および無効になったsnapshotは指定できない。

## 時間計算量

- 戻す更新数を $k$ として $O(k)$

# undo

```cpp
void undo()
```

直前の更新を取り消す。

## 時間計算量

- $O(1)$

# can_undo

```cpp
bool can_undo() const
```

取り消せる更新があるかを返す。

## 時間計算量

- $O(1)$

# history_size

```cpp
int history_size() const
```

現在保持している更新履歴数を返す。

## 時間計算量

- $O(1)$

# size

```cpp
int size() const
```

配列長を返す。

## 時間計算量

- $O(1)$
