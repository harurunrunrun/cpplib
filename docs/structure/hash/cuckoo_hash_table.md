---
title: Cuckoo Hash Table (Cuckooハッシュ表)
documentation_of: ../../../src/structure/hash/cuckoo_hash_table.hpp
---

各キーに 2 つの候補スロットを持ち、エントリを追い出しながら配置するハッシュマップ（Cuckoo hashing）。
与えられたハッシュ関数が異なるキーを区別できない場合にも停止性と正しさを保つため、一定回数の再構築に失敗した後は線形探索のフォールバックに移る。

## API

```cpp
template<class Key, class Value, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class CuckooHashTable;

CuckooHashTable()
bool empty() const
size_t size() const
Value* find(const Key& key)
const Value* find(const Key& key) const
bool contains(const Key& key) const
bool insert_or_assign(const Key& key, const Value& value)
bool erase(const Key& key)
Value& at(const Key& key)
```

- `find`: キーが存在すれば値へのポインタを、存在しなければ `nullptr` を返す。
- `insert_or_assign`: 新しくキーを追加した場合に `true` を、既存の値を更新した場合に `false` を返す。
- `erase`: 削除できた場合に `true` を返す。
- `at`: キーに対応する値への参照を返す。

## 時間計算量

$N$ を要素数とする。

- 通常の Cuckoo 表では、`find`、`contains`、`erase`、`at` は最悪 $O(1)$、`insert_or_assign` は期待償却 $O(1)$。1 回の再ハッシュは $O(N)$
- ハッシュ値が一致する異なるキーを分離できずにフォールバックに移った場合、以降の各操作は最悪 $O(N)$
- `empty`、`size`: $O(1)$

## 空間計算量

$O(N)$。

## 注意点

- 要素数を、各表のスロット数の半分以下に保つ（超える前に表を 2 倍にして再ハッシュする）。
- キーと値はコピー可能でなければならない。
- 存在しないキーへの `at` は `std::out_of_range` を送出する。
- 1 回の再ハッシュで試すシードの数には上限（64 回）があり、退化したハッシュ関数でも無限に再ハッシュすることはない。ただし、一度フォールバックに移ると、要素を削除しても通常の表には戻らない。
