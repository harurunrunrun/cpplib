---
title: Monoid Merge Sort Tree (モノイドマージソート木)
documentation_of: ../../../../src/structure/range_query/value_query/monoid_merge_sort_tree.hpp
---

静的なキーの列と重みの列に対して、添字の区間とキーの上限を同時に指定して重みを集約する。

各重みは可換なモノイドの要素とする。同じキーを持つ要素も別々に集約する。

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

```cpp
template<class Key, auto Monoid, int MAX_SIZE = -1, class Compare = std::less<Key>>
struct MonoidMergeSortTree;
```

- `Key`
    - キーの型。
- `Monoid`
    - `S op(S, S)` と `S e()` を持つ可換なモノイド。
- `MAX_SIZE`
    - 列の長さの上限。省略時（`-1`）は上限を設けず、実行時の長さに合わせて `std::vector` で領域を確保する。
- `Compare`
    - キーの狭義弱順序。省略時は `std::less<Key>`。

## メンバ関数

- `explicit MonoidMergeSortTree(const vector<Key>& keys, const vector<S>& weights, Compare compare = {})`
    - `keys[i]` と `weights[i]` を組にして構築する。
- `explicit MonoidMergeSortTree(const array<Key, N>& keys, const array<S, N>& weights, Compare compare = {})`
    - 固定長配列から構築する。`N <= MAX_SIZE` は `static_assert` で検査する。
- `int size() const`
    - 列の長さを返す。
- `bool empty() const`
    - 列の長さが `0` かどうかを返す。
- `const Key& key(int index) const`
    - `keys[index]` を返す。
- `const S& weight(int index) const`
    - `weights[index]` を返す。
- `S fold(int left, int right) const`
    - `left <= i < right` を満たすすべての `weights[i]` を集約する。
- `S fold_less(int left, int right, const Key& upper) const`
    - `left <= i < right` かつ `compare(keys[i], upper)` を満たす重みを集約する。
- `S fold_less_equal(int left, int right, const Key& upper) const`
    - `left <= i < right` かつ `!compare(upper, keys[i])` を満たす重みを集約する。

## 時間計算量

列の長さを $N$ とする。

- 2 種類のコンストラクタ: $O(N\log N)$
- `size`, `empty`, `key`, `weight`: $O(1)$
- `fold`: $O(\log N)$
- `fold_less`, `fold_less_equal`: $O(\log^2 N)$

## 空間計算量（API 別の追加領域）

- 2 種類のコンストラクタ: $O(N)$
- `fold`, `fold_less`, `fold_less_equal`: $O(\log N)$
- `size`, `empty`, `key`, `weight`: $O(1)$

構築済みのオブジェクトの保存領域は $O(N\log N)$ である。

## 注意点

- `keys.size() == weights.size() <= MAX_SIZE` を要求し、違反時は `std::runtime_error` を送出する。
- `key(index)` と `weight(index)` は `0 <= index < size()` を要求し、違反時は `std::runtime_error` を送出する。
- 3 種類の `fold` は `0 <= left <= right <= size()` を要求し、違反時は `std::runtime_error` を送出する。
- 集約の対象が空なら `Monoid.e()` を返す。
- `Monoid.op` は結合的かつ可換で、`Monoid.e()` は単位元である必要がある。ノード内ではキーの順に重みを保持するので、非可換なモノイドでは元の添字の順序を保てない。
- `Compare` は狭義弱順序である必要がある。コンストラクタに渡した比較オブジェクトが各クエリでも使われる。
