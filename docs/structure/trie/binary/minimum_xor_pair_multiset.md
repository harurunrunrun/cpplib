---
title: Minimum XOR Pair Multiset (最小XOR対付き多重集合) [ABC308_G]
documentation_of: ../../../../src/structure/trie/binary/minimum_xor_pair_multiset.hpp
---

符号なし整数の多重集合を更新しながら、異なる 2 要素を選んだときの XOR の最小値を管理する。
同じ値を複数回格納でき、その 2 個を選んだ XOR は `0` となる。
ソート順で隣接する 2 要素の XOR の多重集合を `std::multiset` で管理する（最小 XOR はソート順で隣接する対で達成される）。

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

`value` を 1 個追加する。

## `erase`

```cpp
bool erase(T value)
```

`value` を 1 個削除する。削除できれば `true` を、存在しなければ変更せず `false` を返す。

## `minimum_pair_xor`

```cpp
T minimum_pair_xor() const
```

集合から異なる 2 要素を選んだときの XOR の最小値を返す。

## 時間計算量

要素数を $N$、`count(value)` の返り値を $C$ とする。

- `size`, `empty`, `minimum_pair_xor`: $O(1)$
- `insert`, `erase`: $O(\log(N+1))$
- `count`: $O(\log(N+1)+C)$

## 空間計算量

$O(N)$ である。

## 注意点

- `minimum_pair_xor` は `size() >= 2` を必要とし、違反時は `runtime_error` を送出する。
- `T` が符号なし整数型でなければコンパイルエラーになる。
