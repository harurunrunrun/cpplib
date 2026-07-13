---
title: Rollback Weighted DSU
documentation_of: ../src/structure/dsu/rollback_weighted_dsu.hpp
---

rollback 可能な差分制約付き素集合データ構造。

# テンプレート引数

```cpp
RollbackWeightedDSU<T, MAX_SIZE, MAX_HISTORY>
```

- ポテンシャル差の型 `T`
- 頂点数の上限 `MAX_SIZE`
- 保存する操作履歴数の上限 `MAX_HISTORY`

`T` は `+`, `-`, `==`, `T()` を持つ。

# コンストラクタ

- $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_HISTORY})$

# merge / diff

```cpp
bool merge(int u, int v, T w)
T diff(int u, int v)
```

`merge(u, v, w)` は `potential(v) - potential(u) = w` を追加する。

矛盾しない場合 `true`、矛盾する場合 `false` を返す。矛盾した場合も履歴は 1 つ進む。

## 時間計算量

- `merge`, `diff`: $O(\log N)$

# rollback

```cpp
int snapshot()
void undo()
void rollback(int snapshot_id)
```

`snapshot` は現在の履歴番号を返す。

`rollback(snapshot_id)` は指定した履歴番号まで戻す。

## 時間計算量

- `snapshot`: $O(1)$
- `undo`: $O(1)$
- `rollback`: 取り消す履歴数を $K$ として $O(K)$

# same / leader

```cpp
bool same(int u, int v)
int leader(int v)
T weight(int v)
int component_size(int v)
int groups()
```

`weight(v)` は `potential(v) - potential(leader(v))` を返す。

## 時間計算量

- $O(\log N)$

`size`, `groups` は $O(1)$。
