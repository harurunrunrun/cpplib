---
title: Persistent Array (完全永続配列)
documentation_of: ../../../src/structure/array/persistent_array.hpp
---

任意の過去の状態を残したまま、1 点を更新した新しい状態を作れる配列（完全永続配列）。

状態は 0 から始まるバージョン番号で指定する。バージョン 0 は初期状態である。

## テンプレート引数

```cpp
PersistentArray<T, MAX_SIZE = -1, MAX_VERSION = -1>
```

- `T`: 要素の型
- `MAX_SIZE`: 配列長の上限。`-1`（省略時）なら上限なし
- `MAX_VERSION`: バージョン 0 から追加できるバージョン数の上限。`-1`（省略時）なら上限なし

`MAX_SIZE`, `MAX_VERSION` を指定すると、初期値・更新値・ノード・バージョン管理の領域を構築時に固定確保する。省略すると、実行時の要素数・更新数に合わせて `std::vector` で確保する可変長版になる。

オブジェクト自体はコピー・ムーブできない。

## コンストラクタ

```cpp
(1) explicit PersistentArray(int n = MAX_SIZE)
(2) PersistentArray(int n, const T& value)
(3) explicit PersistentArray(const std::vector<T>& v)
(4) template<std::size_t N> explicit PersistentArray(const std::array<T, N>& v)
```

1. 長さ `n`、初期値 `T{}` で初期化する。`n` のデフォルトは `MAX_SIZE`（可変長版では 0）。
2. 長さ `n`、初期値 `value` で初期化する。
3. `v` で初期化する。
4. `v` で初期化する。

## set

```cpp
int set(int version, int k, const T& x)
```

バージョン `version` の `k` 番目を `x` に変更した新しいバージョンを作り、その番号を返す。

## fork

```cpp
int fork(int version)
```

`version` と同じ内容の新しいバージョンを作り、その番号を返す。

## get

```cpp
const T& get(int version, int k) const
```

バージョン `version` の `k` 番目を返す。

## size / versions / latest_version

```cpp
int size() const
int versions() const
int latest_version() const
```

順に、配列長、現在存在するバージョン数、最後に作ったバージョンの番号を返す。

## 時間計算量

配列長を $n$、更新回数を $u$、バージョン数を $v$ とし、$H$ を固定長版では $\log\texttt{MAX_SIZE}$、可変長版では $\log(n+1)$ とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_SIZE}+\texttt{MAX_VERSION})$（固定長の初期値・更新値・バージョン管理領域を構築した後、使用する $n$ 要素を初期化する）、可変長版は $O(n)$
- `set`: $O(H)$
- `get`: $O(H)$
- `fork`, `size`, `versions`, `latest_version`: $O(1)$

## 空間計算量

- 保存領域: 固定長版は構築時に固定確保した領域、可変長版は $O(n+uH+v)$
- 追加領域: `set` は経路コピーのノード $O(H)$ 個と再帰スタック $O(H)$ を使う。`get` は反復で辿るので $O(1)$。その他の操作は $O(1)$

## 注意点

- `MAX_SIZE > 0`、`0 <= n <= MAX_SIZE` でなければならない。`std::vector`/`std::array` から構築する場合も長さは `MAX_SIZE` 以下。
- 添字は $[0,n)$、バージョンは $[0,\texttt{versions()})$ でなければならない。範囲外なら `std::runtime_error` を投げる。
- `set` と `fork` はバージョンを 1 個追加する。追加したバージョン数が `MAX_VERSION` を超えると `std::runtime_error` を投げる。
- `set` は経路コピーのノードを $O(H)$ 個生成し、`fork` はノードを生成しない。
- ノード・バージョンの容量超過や値のコピーで例外が起きた場合は、バージョン数とノード数を操作前の状態に戻す。
- 可変長版では、返された参照が後続の更新に伴う再確保で無効になることがある。必要な値は更新前にコピーする。
