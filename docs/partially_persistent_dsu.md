---
title: Partially Persistent DSU (部分永続素集合データ構造)
documentation_of: ../src/structure/dsu/partially_persistent_dsu.hpp
---

過去の状態を参照できる素集合データ構造。

更新は最新状態にのみ行える。

## テンプレート引数

```cpp
PartiallyPersistentDSU<MAX_SIZE, MAX_UPDATE>
```

- 頂点数の上限 `MAX_SIZE`
- 更新回数の上限 `MAX_UPDATE`

## コンストラクタ

```cpp
PartiallyPersistentDSU(int n = MAX_SIZE)
```

version 0 として `n` 頂点の集合を作る。

## 時間計算量

- $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_UPDATE})$

## merge

```cpp
int merge(int u, int v)
```

最新 version で `u` と `v` を併合し、新しい version を返す。

すでに同じ集合でも新しい version を作る。

## 時間計算量

- $O(\log N)$

## same / leader / component_size

```cpp
bool same(int version, int u, int v)
int leader(int version, int v)
int component_size(int version, int v)
int groups(int version)
```

指定した version の情報を返す。

## 時間計算量

- `leader`, `same`: $O(\log N)$
- `component_size`: $O(\log N + \log \mathrm{MAX\_UPDATE})$
- `groups`: $O(1)$

## version

```cpp
int versions()
int latest_version()
```

現在存在する version 数と最新 version を返す。

## 時間計算量

- $O(1)$

`size` も $O(1)$。

## 空間計算量（API別の追加領域）

- constructor、`size`、`versions`、`latest_version`、`leader`、`same`、`component_size`、`groups`: $O(1)$
- `merge`: $O(1)$ の履歴slotを本体へ追加し、呼び出し中の一時領域も $O(1)$

本体の保存領域は $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_UPDATE})$ である。

## 注意点

- `0 <= n <= MAX_SIZE`、頂点は`[0,n)`、versionは`[0,versions())`。違反時は例外。
- version 0が初期状態。`merge`は同一成分の場合も最新状態からversionを1個追加する。
- 更新は高々`MAX_UPDATE`回。超過した`merge`は例外で既存versionを変えない。
- `leader`は代表、`component_size`は成分頂点数、`groups`は成分数を返す。
