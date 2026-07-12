---
title: Partially Persistent Array
documentation_of: ../src/structure/array/partially_persistent_array.hpp
---

過去の状態を参照できる配列。更新できるのは最新の状態だけで、更新前の状態は残る。

状態は0から始まるversionで指定する。version 0は初期状態。

# テンプレート引数

```cpp
PartiallyPersistentArray<T, MAX_SIZE, MAX_UPDATE>
```

- 要素の型 `T`
- 配列長の上限 `MAX_SIZE`
- 更新回数の上限 `MAX_UPDATE`

オブジェクト自体のcopy/moveはできない。

# コンストラクタ

```cpp
(1) PartiallyPersistentArray(int n = MAX_SIZE)
(2) PartiallyPersistentArray(int n, const T& value)
(3) PartiallyPersistentArray(const vector<T>& v)
(4) PartiallyPersistentArray(const array<T, N>& v)
```

1. 長さ `n`、初期値 `T{}` で初期化する。
2. 長さ `n`、初期値 `value` で初期化する。
3. `v` で初期化する。
4. `v` で初期化する。

## 時間計算量

- $O(n)$

# set

```cpp
int set(int k, const T& x)
```

最新状態の `k` 番目を `x` に変更し、新しいversion番号を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_UPDATE})$

# get

```cpp
(1) const T& get(int version, int k) const
(2) const T& get(int k) const
```

1. `version` の `k` 番目を返す。
2. 最新versionの `k` 番目を返す。

## 時間計算量

- (1): $O(\log \mathrm{MAX\_UPDATE})$
- (2): $O(1)$

# version情報

```cpp
int size() const
int versions() const
int latest_version() const
```

順に配列長、version数、最新version番号を返す。

## 時間計算量

- $O(1)$
