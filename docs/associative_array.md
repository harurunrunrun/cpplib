---
title: Associative Array
documentation_of: ../src/structure/hash/associative_array.hpp
---

Robin Hood法によるopen addressing hash table。bucket数は`MAX_SIZE`の2倍以上となる2冪で、領域は構築時に確保する。削除ではbackward shiftを行うためtombstoneを残さない。

# テンプレート引数

```cpp
AssociativeArray<Key, T, MAX_SIZE, Hash, KeyEqual>
```

- `Key`: key型
- `T`: value型
- `MAX_SIZE`: 保持できる要素数
- `Hash`: `std::size_t operator()(const Key&)`を持つhash関数
- `KeyEqual`: keyの等値判定

hash値にはSplitMix64によるmixを行う。

# 構築

```cpp
AssociativeArray(std::uint64_t seed = ..., Hash hash = {}, KeyEqual equal = {})
```

空のtableを構築する。`seed`を変えるとbucket配置が変わる。

## 時間計算量

- $O(\mathtt{capacity})$

# 検索

```cpp
T* find(const Key& key)
const T* find(const Key& key) const
bool contains(const Key& key) const
T& at(const Key& key)
const T& at(const Key& key) const
```

`find`はkeyが存在すればvalueへのpointer、存在しなければ`nullptr`を返す。`at`は存在しないkeyに対して`std::out_of_range`を送出する。

## 時間計算量

- expected $O(1)$
- worst $O(\mathtt{MAX\_SIZE})$

# 追加・更新

```cpp
bool insert_or_assign(Key key, T value)
T& operator[](const Key& key)
T& operator[](Key&& key)
```

`insert_or_assign`は新しくkeyを追加した場合に`true`、既存valueを更新した場合に`false`を返す。`operator[]`は存在しないkeyを`T()`で追加する。要素数が`MAX_SIZE`の状態で新しいkeyを追加すると例外を送出する。

## 時間計算量

- expected $O(1)$
- worst $O(\mathtt{MAX\_SIZE})$

# 削除

```cpp
bool erase(const Key& key)
void clear()
```

`erase`は削除できた場合に`true`を返す。`clear`は全要素を削除する。

## 時間計算量

- `erase`: expected $O(1)$、worst $O(\mathtt{MAX\_SIZE})$
- `clear`: $O(\mathtt{capacity})$

# 要素数

```cpp
int size() const
bool empty() const
static constexpr int max_size()
static constexpr std::size_t capacity()
```

## 時間計算量

- すべて $O(1)$

# 空間計算量

- $O(\mathtt{MAX\_SIZE})$

copyは無効で、moveに対応する。
