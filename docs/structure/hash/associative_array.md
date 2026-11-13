---
title: Associative Array (連想配列) [associative_array]
documentation_of: ../../../src/structure/hash/associative_array.hpp
---

Robin Hood 法によるオープンアドレス法のハッシュテーブル。削除では後方シフトを行うため、墓標（tombstone）を残さない。

固定長版のバケット数は $2 \cdot \texttt{MAX_SIZE}$ 以上の最小の 2 冪で、領域は構築時にヒープへ確保する。

## テンプレート引数

```cpp
AssociativeArray<Key, T, MAX_SIZE = -1, Hash = std::hash<Key>, KeyEqual = std::equal_to<Key>>
```

- `Key`: キーの型
- `T`: 値の型
- `MAX_SIZE`: 保持できる要素数の上限。省略（`-1`）すると、`std::vector` を使って表を伸長する可変長版になる
- `Hash`: `std::size_t operator()(const Key&)` を持つハッシュ関数
- `KeyEqual`: キーの等値判定

ハッシュ値は、シードを加えてから SplitMix64 で混ぜてバケットを決める。

```cpp
using Variable = AssociativeArray<Key, T>;
using Fixed = AssociativeArray<Key, T, 200000>;
```

## コンストラクタ

```cpp
explicit AssociativeArray(std::uint64_t seed = 0x243f6a8885a308d3, Hash hash = {}, KeyEqual equal = {})
```

空の表を構築する。`seed` を変えるとバケットの配置が変わる。
コピーはできず、ムーブはできる。

## 検索

```cpp
T* find(const Key& key)
const T* find(const Key& key) const
bool contains(const Key& key) const
T& at(const Key& key)
const T& at(const Key& key) const
```

`find` はキーが存在すれば値へのポインタを、存在しなければ `nullptr` を返す。`at` は存在しないキーに対して `std::out_of_range` を送出する。

## 追加・更新

```cpp
bool insert_or_assign(Key key, T value)
T& operator[](const Key& key)
T& operator[](Key&& key)
```

`insert_or_assign` は新しくキーを追加した場合に `true` を、既存の値を更新した場合に `false` を返す。`operator[]` は存在しないキーを `T()` で追加する。

## 削除

```cpp
bool erase(const Key& key)
void clear()
```

`erase` は削除できた場合に `true` を返す。`clear` は全要素を削除する。

## 要素数

```cpp
int size() const
bool empty() const
static constexpr int max_size()
capacity()
```

`max_size` は `MAX_SIZE`（可変長版では `INT_MAX`）を返す。`capacity` は表のスロット数を返す。固定長版では `static constexpr std::size_t capacity()`、可変長版では現在のスロット数を返す非静的メンバ関数である。

## 時間計算量

$C$ を表のスロット数とする。

- コンストラクタ: $O(C)$（可変長版の初期スロット数は $16$）
- `find`、`contains`、`at`、`insert_or_assign`、`operator[]`、`erase`: 期待 $O(1)$、最悪 $O(C)$
- `clear`: $O(C)$
- `size`、`empty`、`max_size`、`capacity`: $O(1)$

可変長版は、負荷率が $1/2$ を超える前に表を 2 倍にして再ハッシュする。1 回の再ハッシュは $O(N)$ だが、良いハッシュ関数のもとで検索・挿入・削除は期待償却 $O(1)$ となる。

## 空間計算量

- 固定長版: $O(\texttt{MAX_SIZE})$
- 可変長版: 要素数 $N$ に対して $O(N)$
- コンストラクタ・`clear` 以外の各関数の追加領域は $O(1)$（新しい要素のキー・値は本体の保存領域として数える）

## 注意点

- `Hash` と `KeyEqual` は、等しいキーに同じハッシュ値を与えなければならない。
- 存在しないキーへの `at` は `std::out_of_range` を、固定長版で要素数が `MAX_SIZE` の状態での新しいキーの挿入は `std::runtime_error` を送出する。可変長版で `int` の表現上限を超える要素数も例外となる。
- `find` が返すポインタと、`at`・`operator[]` が返す参照は、`erase`・`clear`・ムーブ・破棄の後は使えない。可変長版では、再ハッシュを起こし得る挿入の後も使えない。
- Robin Hood 法の挿入・削除で要素は移動するため、他の要素の挿入・削除の後もポインタ・参照が同じ要素を指すとは限らない。
