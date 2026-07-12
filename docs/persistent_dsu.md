---
title: Persistent Disjoint Set Union
documentation_of: ../src/structure/dsu/persistent_dsu.hpp
---

完全永続素集合データ構造。

各バージョンの `parent_or_size` を永続配列で持つ。`merge` は指定したバージョンから新しいバージョンを作る。

# テンプレート引数

```cpp
PersistentDSU<MAX_SIZE, MAX_VERSION>
```

- 頂点数の上限 `MAX_SIZE`
- 追加できるバージョン数の上限 `MAX_VERSION`

# コンストラクタ

```cpp
PersistentDSU(int n = MAX_SIZE)
```

`n` 頂点の初期バージョン `0` を作る。

## 時間計算量

- $O(1)$

# merge / fork

```cpp
int merge(int version, int u, int v)
int fork(int version)
```

`merge` は `version` での `u, v` を併合した新しいバージョン番号を返す。
すでに同じ集合の場合も新しいバージョンを作る。

`fork` は同じ状態を指す新しいバージョンを作る。

## 時間計算量

- `merge`: $O(\log^2 N + \log MAX\_SIZE)$
- `fork`: $O(1)$

# same / leader / component_size

```cpp
bool same(int version, int u, int v)
int leader(int version, int v)
int component_size(int version, int v)
int groups(int version)
```

指定したバージョンで問い合わせる。

## 時間計算量

- `same`, `leader`, `component_size`: $O(\log^2 N)$
- `groups`: $O(1)$

# versions

```cpp
int versions()
int latest_version()
int nodes_used()
```

作られたバージョン数、最後に作られたバージョン番号、使用中の永続配列ノード数を返す。

## 時間計算量

- $O(1)$
