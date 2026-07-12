---
title: Rollback DSU
documentation_of: ../src/structure/dsu/rollback_dsu.hpp
---

rollback 可能な素集合データ構造。

# テンプレート引数

```cpp
RollbackDSU<MAX_SIZE, MAX_HISTORY>
```

- 頂点数の上限 `MAX_SIZE`
- 履歴数の上限 `MAX_HISTORY`

# コンストラクタ

```cpp
RollbackDSU(int n = MAX_SIZE)
```

`n` 頂点の集合を作る。

## 時間計算量

- $O(N)$

# merge / same

```cpp
bool merge(int u, int v)
bool same(int u, int v)
```

`merge` は異なる集合が併合されたとき `true` を返す。

## 時間計算量

- $O(\log N)$

# snapshot / rollback

```cpp
int snapshot()
void rollback(int snapshot_id)
void undo()
```

`snapshot` は現在の履歴位置を返す。

`rollback(snapshot_id)` はその履歴位置まで戻す。

`undo()` は直前の `merge` を取り消す。

## 時間計算量

- `snapshot`: $O(1)$
- `rollback`: 戻す `merge` 1 個あたり $O(1)$
- `undo`: $O(1)$

# leader / component_size

```cpp
int leader(int v)
int component_size(int v)
int groups()
```

## 時間計算量

- $O(\log N)$
