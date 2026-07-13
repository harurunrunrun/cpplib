---
title: Persistent Array
documentation_of: ../src/structure/array/persistent_array.hpp
---

任意の過去の状態を残したまま、1点更新した新しい状態を作る配列。

状態は0から始まるversionで指定する。version 0は初期状態。

# テンプレート引数

```cpp
PersistentArray<T, MAX_SIZE, MAX_VERSION>
```

- 要素の型 `T`
- 配列長の上限 `MAX_SIZE`
- version 0から追加できるversion数の上限 `MAX_VERSION`

オブジェクト自体のcopy/moveはできない。

# コンストラクタ

```cpp
(1) PersistentArray(int n = MAX_SIZE)
(2) PersistentArray(int n, const T& value)
(3) PersistentArray(const vector<T>& v)
(4) PersistentArray(const array<T, N>& v)
```

1. 長さ `n`、初期値 `T{}` で初期化する。
2. 長さ `n`、初期値 `value` で初期化する。
3. `v` で初期化する。
4. `v` で初期化する。

## 時間計算量

- $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION})$

固定長の初期値・更新値・version管理領域を構築した後、使用する $n$ 要素を初期化する。

# set

```cpp
int set(int version, int k, const T& x)
```

`version` の `k` 番目を `x` に変更した新しいversionを作り、その番号を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# fork

```cpp
int fork(int version)
```

`version` と同じ内容の新しいversionを作り、その番号を返す。

## 時間計算量

- $O(1)$

# get

```cpp
const T& get(int version, int k) const
```

`version` の `k` 番目を返す。

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

最後に作ったversionの番号を返す。

## 時間計算量

- $O(1)$

# 前提・例外・容量

- `MAX_SIZE > 0`、`0 <= n <= MAX_SIZE`。vector/array入力も`MAX_SIZE`以下。
- indexは`[0,n)`、versionは`[0,versions())`。範囲外なら例外。
- `set`と`fork`はversionを1個追加し、追加version数が`MAX_VERSION`を超えると例外。
- `set`はpath-copy nodeを$O(\log MAX\_SIZE)$個生成し、`fork`はnodeを生成しない。
- node/version容量超過または値のcopy例外時は、version数とnode数を操作前へ戻す。
