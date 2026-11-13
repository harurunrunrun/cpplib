---
title: Robin Hood Hash Table (Robin Hoodハッシュ表)
documentation_of: ../../../src/structure/hash/robin_hood_hash_table.hpp
---

探索距離（probe distance）が大きいエントリを優先して配置する、オープンアドレス法のハッシュマップ（Robin Hood hashing）。削除は後方シフトで行う。

## API

```cpp
template<class Key, class Value, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class RobinHoodHashTable;

RobinHoodHashTable()
explicit RobinHoodHashTable(size_t capacity)
bool empty() const
size_t size() const
bool insert_or_assign(const Key& key, const Value& value)
bool insert_or_assign(Key&& key, Value&& value)
Value* find(const Key& key)
const Value* find(const Key& key) const
bool contains(const Key& key) const
Value& at(const Key& key)
const Value& at(const Key& key) const
bool erase(const Key& key)
```

- コンストラクタの `capacity` は初期のスロット数で、$8$ 以上の 2 冪に切り上げる（デフォルトは $8$）。
- `insert_or_assign`: 新しくキーを追加した場合に `true` を、既存の値を更新した場合に `false` を返す。
- `find`: キーが存在すれば値へのポインタを、存在しなければ `nullptr` を返す。
- `erase`: 削除できた場合に `true` を返す。

## 時間計算量

$N$ を要素数とする。

- 良いハッシュ関数を仮定すると、検索・追加・削除は期待償却 $O(1)$。最悪 $O(N)$
- 1 回の再ハッシュは $O(N)$
- `empty`、`size`: $O(1)$

## 空間計算量

$O(N)$。

## 注意点

- 最大の負荷率は $0.8$ で、これを超える前に表を 2 倍にして再ハッシュする。
- バケットは `Hash` の値の下位ビットだけで決まり、追加の混ぜ込みは行わない。`std::hash<int>` のように恒等写像に近いハッシュでは、下位ビットが揃ったキーで性能が落ちる。
- 存在しないキーへの `at` は `std::out_of_range` を送出する。
