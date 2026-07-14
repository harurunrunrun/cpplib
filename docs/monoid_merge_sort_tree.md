---
title: Monoid Merge Sort Tree (モノイドマージソート木)
documentation_of: ../src/structure/other/monoid_merge_sort_tree.hpp
---

静的な key 列と重み列に対し、index 区間と key 上限を同時に指定して重みを集約する。

各重みは可換 Monoid の要素とする。同じ key を持つ要素も別々に集約する。

## 使い方

```cpp
constexpr AddMonoid<long long> add_monoid{};
MonoidMergeSortTree<int, add_monoid, 200000> tree(keys, weights);

// left <= i < right かつ keys[i] < upper
long long x = tree.fold_less(left, right, upper);

// left <= i < right かつ keys[i] <= upper
long long y = tree.fold_less_equal(left, right, upper);
```

## テンプレート引数

- `Key`
    - key の型。
- `Monoid`
    - `S op(S,S)` と `S e()` を持つ可換 Monoid。
- `MAX_SIZE`
    - 列長の上限。
- `Compare`
    - key の狭義弱順序。省略時は `std::less<Key>`。

## メンバ関数

- `MonoidMergeSortTree(vector<Key> keys, vector<S> weights, Compare compare = {})`
    - `keys[i]` と `weights[i]` を組にして構築する。
- `MonoidMergeSortTree(array<Key,N> keys, array<S,N> weights, Compare compare = {})`
    - 固定長配列から構築する。
- `int size() const`
    - 列長を返す。
- `bool empty() const`
    - 列長が `0` かを返す。
- `const Key& key(int index) const`
    - `keys[index]` を返す。
- `const S& weight(int index) const`
    - `weights[index]` を返す。
- `S fold(int left, int right) const`
    - `left <= i < right` を満たすすべての `weights[i]` を集約する。
- `S fold_less(int left, int right, const Key& upper) const`
    - `left <= i < right` かつ `Compare{}(keys[i], upper)` を満たす重みを集約する。
- `S fold_less_equal(int left, int right, const Key& upper) const`
    - `left <= i < right` かつ `!Compare{}(upper, keys[i])` を満たす重みを集約する。

## 時間計算量

- 2 種類の constructor: $O(N\log N)$
- `size`, `empty`, `key`, `weight`: $O(1)$
- `fold`: $O(\log N)$
- `fold_less`, `fold_less_equal`: $O(\log^2 N)$

## 空間計算量（API 別の追加領域）

- 2 種類の constructor: $O(N)$
- `fold`, `fold_less`, `fold_less_equal`: $O(\log N)$
- `size`, `empty`, `key`, `weight`: $O(1)$

構築済み object の保存領域は $O(N\log N)$。

## 注意点

- `keys.size() == weights.size() <= MAX_SIZE`。違反時は例外。
- `key(index)` と `weight(index)` は `0 <= index < size()`。違反時は例外。
- 3 種類の `fold` は `0 <= left <= right <= size()`。違反時は例外。
- 空の対象集合は `Monoid.e()` を返す。
- `Monoid.op` は結合的かつ可換で、`Monoid.e()` は単位元である必要がある。node 内では key 順に重みを保持するため、非可換 Monoid では元の index 順を保存できない。
- `Compare` は狭義弱順序である必要がある。constructor に渡した比較 object が各 query でも使われる。
