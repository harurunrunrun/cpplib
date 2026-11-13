---
title: Rollback Weighted DSU (ロールバック可能重み付き素集合データ構造)
documentation_of: ../../../../src/structure/dsu/rollback/rollback_weighted_dsu.hpp
---

ロールバック可能な差分制約付き（重み付き）素集合データ構造。経路圧縮を行わず、union by size を使う。

## テンプレート引数

```cpp
RollbackWeightedDSU<T, MAX_SIZE = -1, MAX_HISTORY = -1>
```

- `T`: ポテンシャルの差の型。`+`、`-`、`==`、デフォルト構築 `T()`（零元）を持つ
- `MAX_SIZE`: 頂点数の上限
- `MAX_HISTORY`: 保存する操作履歴数（`merge` の呼び出し回数）の上限

省略（`-1`）した上限は、実行時の要素数・履歴数に合わせて `std::vector` を確保する可変長になる。

```cpp
using Dynamic = RollbackWeightedDSU<long long>;
```

## コンストラクタ

```cpp
explicit RollbackWeightedDSU(int n = MAX_SIZE)
```

$n$ 頂点の集合を作る。可変長版では `n` のデフォルトは $0$ である。

## merge / diff

```cpp
bool merge(int u, int v, T w)
T diff(int u, int v) const
```

- `merge(u, v, w)`: 制約 $\mathrm{potential}(v) - \mathrm{potential}(u) = w$ を追加する。矛盾しない場合は `true`、矛盾する場合は `false` を返す。併合・既存の制約との一致・矛盾のいずれの場合も履歴は 1 個進む。
- `diff(u, v)`: $\mathrm{potential}(v) - \mathrm{potential}(u)$ を返す。

## snapshot / undo / rollback

```cpp
int snapshot() const
void undo()
void rollback(int snapshot_id)
```

- `snapshot()`: 現在の履歴数を返す
- `undo()`: 直前の `merge` を 1 回取り消す
- `rollback(snapshot_id)`: 履歴数が `snapshot_id` になるまで戻す

## same / leader / weight

```cpp
bool same(int u, int v) const
int leader(int v) const
T weight(int v) const
int component_size(int v) const
int size() const
int groups() const
```

`weight(v)` は $\mathrm{potential}(v) - \mathrm{potential}(\mathrm{leader}(v))$ を返す。

## 時間計算量

- コンストラクタ: $O(N)$。固定長版では配列の初期化に $O(\texttt{MAX_SIZE}+\texttt{MAX_HISTORY})$ かかる
- `merge`、`diff`、`same`、`leader`、`weight`、`component_size`: 最悪 $O(\log N)$
- `snapshot`、`undo`、`size`、`groups`: $O(1)$
- `rollback`: 取り消す履歴数を $K$ として $O(K)$

## 空間計算量

- 本体: $O(N + H)$（$H$ は履歴数。固定長版では $O(\texttt{MAX_SIZE}+\texttt{MAX_HISTORY})$）
- 各 `merge` は履歴を 1 個消費する
- 各関数の追加領域: $O(1)$

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ で、頂点は $[0, n)$ でなければならない。違反すると `std::runtime_error` を送出する。
- `diff(u, v)` は同じ成分であることを前提とし、実装は連結性を検査しない。
- 履歴数が `MAX_HISTORY` に達した後の `merge`、履歴が空の `undo`、範囲外の `rollback` は `std::runtime_error` を送出する。
