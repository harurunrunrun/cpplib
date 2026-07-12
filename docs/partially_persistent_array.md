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

- (1), (2): $O(n)$
- (3), (4): $O(|v|)$

# set

```cpp
int set(int k, const T& x)
```

最新の状態の `k` 番目を `x` に変更し、新しいversionの番号を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# get

```cpp
(1) const T& get(int version, int k) const
(2) const T& get(int k) const
```

1. `version` の `k` 番目を返す。
2. 最新のversionの `k` 番目を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# size

```cpp
int size() const
```

配列長を返す。

## 時間計算量

- $O(1)$

# versions

```cpp
int versions() const
```

現在存在するversion数を返す。

## 時間計算量

- $O(1)$

# latest_version

```cpp
int latest_version() const
```

最新のversion番号を返す。

## 時間計算量

- $O(1)$
