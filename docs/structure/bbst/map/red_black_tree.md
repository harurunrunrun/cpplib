---
title: Red Black Tree (赤黒木) [ordered_set] [ORDERSET]
documentation_of: ../../../../src/structure/bbst/map/red_black_tree.hpp
---

赤黒木（左傾赤黒木）による順序付き集合。

## テンプレート引数

```cpp
RedBlackSet<Key, MAX_SIZE = -1, Compare = std::less<Key>>
```

- `Key`: キーの型
- `MAX_SIZE`: 同時に保持する要素数の上限。`-1`（省略時）なら上限なしの可変長版
- `Compare`: 比較関数

`MAX_SIZE` を指定するとノード領域を固定確保し、省略するとノードを `std::vector` に必要に応じて確保する（伸長コストは償却 $O(1)$）。コピーはできないが、ムーブはできる。

同じキーは 1 個だけ保持する。

## コンストラクタ

```cpp
RedBlackSet(const Compare& compare = Compare())
int size() const
bool empty() const
void clear()
```

## insert / erase / contains

```cpp
bool insert(const Key& x)
bool erase(const Key& x)
bool contains(const Key& x) const
```

`insert` はすでに存在する場合 `false` を返す。

`erase` は存在しない場合 `false` を返す。

## 順序

```cpp
int order_of_key(const Key& x) const
int order_of_key_upper(const Key& x) const
std::optional<Key> kth(int k) const
```

`order_of_key(x)` は `x` 未満の要素数を返す。

`order_of_key_upper(x)` は `x` 以下の要素数を返す。

`kth(k)` は 0-indexed で `k` 番目の要素を返す。存在しない場合 `std::nullopt` を返す。

## 探索

```cpp
std::optional<Key> lower_bound(const Key& x) const
std::optional<Key> upper_bound(const Key& x) const
std::optional<Key> max_leq(const Key& x) const
std::optional<Key> max_less(const Key& x) const
```

順に、`x` 以上の最小要素、`x` より大きい最小要素、`x` 以下の最大要素、`x` 未満の最大要素を返す。存在しない場合 `std::nullopt` を返す。

## 時間計算量

$N$ を現在の要素数とする。比較と `Key` のコピーは $O(1)$ とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_SIZE})$（固定長領域を確保・初期化する）、可変長版は $O(1)$
- `size`, `empty`, `clear`: $O(1)$
- `contains`, `insert`, `erase`: 最悪 $O(\log(N+1))$
- `order_of_key`, `order_of_key_upper`, `kth`: 最悪 $O(\log(N+1))$
- `lower_bound`, `upper_bound`, `max_leq`, `max_less`: 最悪 $O(\log(N+1))$

## 空間計算量

- オブジェクト全体: 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は一度でも確保したノード数に比例
- 各操作の追加領域: 再帰スタックを含めて $O(\log(N+1))$

## 注意点

- `Compare` は狭義弱順序でなければならない。同値なキーは 1 個だけ保持する。
- `insert` は既存のキーなら `false` を返す。要素数が `MAX_SIZE` の状態で新規キーを挿入すると `std::runtime_error` を投げる。
- `clear` は $O(1)$ で、確保済みのノード領域を再利用する。
