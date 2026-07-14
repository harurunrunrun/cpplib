---
title: Minimum XOR Pair Multiset (最小XOR対付き多重集合) [ABC308_G]
documentation_of: ../src/structure/trie/minimum_xor_pair_multiset.hpp
---

符号なし整数の多重集合を更新しながら、異なる2要素を選んだときのXORの最小値を管理する。
同じ値を複数回格納でき、その2個を選んだXORは `0` となる。

## 型

```cpp
MinimumXorPairMultiset<T>
```

`T` は符号なし整数型でなければならない。

## `size`

```cpp
size_t size() const
```

重複を含む要素数を返す。

## `empty`

```cpp
bool empty() const
```

要素がなければ `true` を返す。

## `count`

```cpp
size_t count(T value) const
```

`value` の格納個数を返す。

## `insert`

```cpp
void insert(T value)
```

`value` を1個追加する。

## `erase`

```cpp
bool erase(T value)
```

`value` を1個削除する。存在しなければ変更せず `false`、削除できれば `true` を返す。

## `minimum_pair_xor`

```cpp
T minimum_pair_xor() const
```

集合から異なる2要素を選んだときのXORの最小値を返す。

## 時間計算量

要素数を $N$、`count(value)` の返り値を $C$ とする。

- `size`, `empty`, `minimum_pair_xor`: $O(1)$
- `insert`, `erase`: $O(\log(N+1))$
- `count`: $O(\log(N+1)+C)$

## 空間計算量

$O(N)$。

## 注意点

- `minimum_pair_xor` は `size() >= 2` を必要とし、違反時は `runtime_error`。
