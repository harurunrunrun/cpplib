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

- $O(\mathtt{MAX\_VERSION})$

# merge / fork

```cpp
int merge(int version, int u, int v)
int fork(int version)
```

`merge` は `version` での `u, v` を併合した新しいバージョン番号を返す。
すでに同じ集合の場合も新しいバージョンを作る。

`fork` は同じ状態を指す新しいバージョンを作る。

## 時間計算量

- `merge`: $O(\log N\log \mathtt{MAX\_SIZE})$
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

- `same`, `leader`, `component_size`: $O(\log N\log \mathtt{MAX\_SIZE})$
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

`size`, `nodes_used` も $O(1)$。

# 前提・例外・容量

- `0 <= n <= MAX_SIZE`、頂点は`[0,n)`、versionは`[0,versions())`。違反時は例外。
- `fork`と`merge`はversionを1個追加し、追加version数が`MAX_VERSION`を超えると例外。
- 異なる成分の`merge`は永続配列nodeを$O(\log MAX\_SIZE)$個生成する。
- 同じ成分の`merge`と`fork`はnodeを生成しない。
- node容量超過時はnode数とversion数を操作前へ戻す。
