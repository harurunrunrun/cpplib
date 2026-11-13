---
title: Rollback DSU (ロールバック可能素集合データ構造)
documentation_of: ../../../../src/structure/dsu/rollback/rollback_dsu.hpp
---

ロールバック可能な素集合データ構造。経路圧縮を行わず、union by size で木の高さを $O(\log N)$ に保つ。

## テンプレート引数

```cpp
RollbackDSU<MAX_SIZE = -1, MAX_HISTORY = -1>
```

- `MAX_SIZE`: 頂点数の上限
- `MAX_HISTORY`: 履歴数（`merge` の呼び出し回数）の上限

省略（`-1`）した上限は、実行時の要素数・履歴数に合わせて `std::vector` を確保する可変長になる。

```cpp
using Dynamic = RollbackDSU<>;
```

## コンストラクタ

```cpp
explicit RollbackDSU(int n = MAX_SIZE)
```

$n$ 頂点の集合を作る。可変長版では `n` のデフォルトは $0$ である。

## merge / same

```cpp
bool merge(int u, int v)
bool same(int u, int v) const
```

`merge` は異なる集合が併合されたとき `true` を返す。同じ成分どうしの `merge` も履歴を 1 個保存する。

## snapshot / rollback / undo

```cpp
int snapshot() const
void rollback(int snapshot_id)
void undo()
```

- `snapshot()`: 現在の履歴数を返す
- `rollback(snapshot_id)`: 履歴数が `snapshot_id` になるまで戻す
- `undo()`: 直前の `merge` を 1 回取り消す

## leader / component_size / size / groups

```cpp
int leader(int v) const
int component_size(int v) const
int size() const
int groups() const
```

代表元、成分の頂点数、頂点数、成分数を返す。

## 時間計算量

- コンストラクタ: $O(N)$。固定長版では配列の初期化に $O(\texttt{MAX_SIZE}+\texttt{MAX_HISTORY})$ かかる
- `merge`、`same`、`leader`、`component_size`: $O(\log N)$
- `snapshot`、`undo`、`size`、`groups`: $O(1)$
- `rollback`: 取り消す履歴数を $K$ として $O(K)$

## 空間計算量

- 本体: $O(N + H)$（$H$ は履歴数。固定長版では $O(\texttt{MAX_SIZE}+\texttt{MAX_HISTORY})$）
- 各 `merge` は履歴を 1 個消費する
- `leader`、`same`、`component_size`、`merge` の追加領域: 代表探索の再帰を含めて $O(\log N)$
- それ以外の関数の追加領域: $O(1)$

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ で、頂点は $[0, n)$ でなければならない。違反すると `std::runtime_error` を送出する。
- 履歴数が `MAX_HISTORY` に達した後の `merge` は `std::runtime_error` を送出する。
- `rollback(s)` は $0 \le s \le \mathrm{snapshot}()$ を要求する。範囲外の `rollback` と、履歴が空の `undo()` は `std::runtime_error` を送出する。
