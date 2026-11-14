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
- バケットは最初は `Hash` の値の下位ビットで決める。挿入で置いた要素の探索距離が $32$ を超えるか、探索距離の平均が $4$ を超えたら、以後は `Hash` の値を SplitMix64 の混ぜ込み関数に通してから下位ビットで決めるよう切り替え、同じスロット数で再ハッシュする（切り替えは表ごとに高々 1 回で、元に戻すことはない）。
  `std::hash<int>` のように恒等写像に近いハッシュでも、連番の整数キーは混ぜ込みなしで連続したメモリを順にたどれて速く、下位ビットが揃ったキーでは切り替えにより極端に遅くなることはない（混ぜ込みは固定なので、意図的に衝突させる入力は防げない）。
- 存在しないキーへの `at` は `std::out_of_range` を送出する。
