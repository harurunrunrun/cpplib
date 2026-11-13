---
title: Partially Persistent Array (部分永続配列)
documentation_of: ../../../src/structure/array/partially_persistent_array.hpp
---

過去の状態を参照できる配列（部分永続配列）。更新できるのは最新の状態だけで、更新前の状態は残る。

状態は 0 から始まるバージョン番号で指定する。バージョン 0 は初期状態である。

## テンプレート引数

```cpp
PartiallyPersistentArray<T, MAX_SIZE = -1, MAX_UPDATE = -1>
```

- `T`: 要素の型
- `MAX_SIZE`: 配列長の上限。`-1`（省略時）なら上限なし
- `MAX_UPDATE`: 更新回数の上限。`-1`（省略時）なら上限なし

`MAX_SIZE`, `MAX_UPDATE` を指定すると、初期値 `MAX_SIZE` 個分と変更値 `MAX_UPDATE` 個分の領域を構築時に固定確保する。省略すると、実行時の要素数・更新数に合わせて `std::vector` で確保する可変長版になる。

オブジェクト自体はコピー・ムーブできない。

## コンストラクタ

```cpp
(1) explicit PartiallyPersistentArray(int n = MAX_SIZE)
(2) PartiallyPersistentArray(int n, const T& value)
(3) explicit PartiallyPersistentArray(const std::vector<T>& v)
(4) template<std::size_t N> explicit PartiallyPersistentArray(const std::array<T, N>& v)
```

1. 長さ `n`、初期値 `T{}` で初期化する。`n` のデフォルトは `MAX_SIZE`（可変長版では 0）。
2. 長さ `n`、初期値 `value` で初期化する。
3. `v` で初期化する。
4. `v` で初期化する。

## set

```cpp
int set(int k, const T& x)
```

最新状態の `k` 番目を `x` に変更し、新しいバージョン番号を返す。

## get

```cpp
(1) const T& get(int version, int k) const
(2) const T& get(int k) const
```

1. バージョン `version` の `k` 番目を返す。
2. 最新バージョンの `k` 番目を返す。

## バージョン情報

```cpp
int size() const
int versions() const
int latest_version() const
```

順に、配列長、バージョン数、最新のバージョン番号を返す。

## 時間計算量

配列長を $n$、添字 `k` の変更回数を $U_k$、総更新回数を $u$ とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE})$（固定長の値領域と各添字の履歴列を構築した後、使用する $n$ 要素を初期化する）、可変長版は $O(n)$
- `set`: 償却 $O(1)$
- `get(version, k)`: $O(\log U_k)$（最新版を指す場合は $O(1)$）
- `get(k)`, `size`, `versions`, `latest_version`: $O(1)$

## 空間計算量

- 固定長版: $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE})$
- 可変長版: $O(n+u)$

## 注意点

- `0 <= n <= MAX_SIZE`、添字は $[0,n)$、バージョンは $[0,\texttt{versions()})$ でなければならない。違反すると `std::runtime_error` を投げる。
- 更新回数が `MAX_UPDATE` に達した後の `set` は `std::runtime_error` を投げる。
- 容量超過や値のコピーで失敗した更新はバージョンを追加せず、既存の状態を変えない。
- 可変長版では、返された参照が後続の更新に伴う再確保で無効になることがある。必要な値は更新前にコピーする。
