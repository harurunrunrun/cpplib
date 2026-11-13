---
title: Rollback Array (ロールバック可能配列)
documentation_of: ../../../src/structure/array/rollback_array.hpp
---

1 点更新を取り消せる配列。

## テンプレート引数

```cpp
RollbackArray<T, MAX_SIZE = -1, MAX_UPDATE = -1>
```

- `T`: 要素の型
- `MAX_SIZE`: 配列長の上限。`-1`（省略時）なら上限なし
- `MAX_UPDATE`: 同時に保持する更新履歴数の上限。`-1`（省略時）なら上限なし

`MAX_SIZE`, `MAX_UPDATE` を指定すると、初期値・更新値・履歴の領域を構築時に固定確保する。省略すると、実行時の要素数・更新数に合わせて `std::vector` で確保する可変長版になる。

オブジェクト自体はコピー・ムーブできない。

## コンストラクタ

```cpp
(1) explicit RollbackArray(int n = MAX_SIZE)
(2) RollbackArray(int n, const T& value)
(3) explicit RollbackArray(const std::vector<T>& v)
(4) template<std::size_t N> explicit RollbackArray(const std::array<T, N>& v)
```

1. 長さ `n`、初期値 `T{}` で初期化する。`n` のデフォルトは `MAX_SIZE`（可変長版では 0）。
2. 長さ `n`、初期値 `value` で初期化する。
3. `v` で初期化する。
4. `v` で初期化する。

## set / get

```cpp
void set(int k, const T& x)
const T& get(int k) const
```

`set` は `k` 番目を `x` に変更し、変更前の値を履歴に追加する。`get` は `k` 番目を返す。

## snapshot / rollback

```cpp
using Snapshot = int;

Snapshot snapshot() const
void rollback(Snapshot s)
```

`snapshot` は現在の更新履歴数を返す。`rollback(s)` は `snapshot()` が `s` を返した時点まで更新を戻す。

`rollback` で取り消した履歴の上で取った snapshot は無効になる。現在より後の snapshot や無効になった snapshot は指定できない。

## undo / can_undo

```cpp
void undo()
bool can_undo() const
```

`undo` は直前の更新を取り消す。`can_undo` は取り消せる更新があるかを返す。

## history_size / size

```cpp
int history_size() const
int size() const
```

現在保持している更新履歴数と、配列長を返す。

## 時間計算量

配列長を $n$ とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE})$（固定長の領域を構築した後、使用する $n$ 要素を初期化する）、可変長版は $O(n)$
- `set`: $O(1)$（可変長版は償却 $O(1)$）
- `rollback(s)`: 戻す更新数を $k$ として $O(k)$
- `get`, `snapshot`, `undo`, `can_undo`, `history_size`, `size`: $O(1)$

## 空間計算量

- 保存領域: 固定長版は $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE})$、可変長版は保存中の更新数を $u$ として $O(n+u)$
- 各操作の追加領域は $O(1)$（`set` は履歴を 1 個消費する）

## 注意点

- `0 <= n <= MAX_SIZE`、添字は $[0,n)$ でなければならない。範囲外なら `std::runtime_error` を投げる。
- `set` は履歴を 1 個追加する。履歴数が `MAX_UPDATE` に達していると `std::runtime_error` を投げる。
- `undo` は `can_undo()` が真であることを要求し、履歴が空なら `std::runtime_error` を投げる。
- `rollback(s)` は `0 <= s <= history_size()` を要求し、範囲外なら `std::runtime_error` を投げる。
- 更新値の構築に失敗した場合は、履歴と現在値を操作前の状態に戻す。
- 可変長版では、返された参照が後続の更新に伴う再確保で無効になることがある。必要な値は更新前にコピーする。
