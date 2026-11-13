---
title: Treap (トレープ) [ordered_set] [point_set_range_frequency]
documentation_of: ../../../../src/structure/tree/treap/treap.hpp
---

乱択平衡二分探索木（treap）による順序付き集合である。

## テンプレート引数

```cpp
TreapSet<Key, MAX_SIZE, Compare>
```

- キーの型 `Key`
- 同時に保持する要素数の上限 `MAX_SIZE`（省略するとデフォルト値 `-1` の可変長版）
- 比較関数 `Compare`

同じキーは 1 個だけ保持する。

## コンストラクタ

```cpp
TreapSet(uint64_t seed = ..., const Compare& compare = Compare())
```

優先度の生成に `seed` を使う。

## 時間計算量

- 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は $O(1)$

固定長版は容量分のキー・ノード領域を構築する。

## `insert` / `erase` / `contains`

```cpp
bool insert(const Key& x)
bool erase(const Key& x)
bool contains(const Key& x)
```

`insert` は追加できれば `true` を、既に存在する場合は `false` を返す。

`erase` は削除できれば `true` を、存在しない場合は `false` を返す。`contains` は存在するかを返す。

## 時間計算量

- 期待 $O(\log N)$

## 順序

```cpp
int order_of_key(const Key& x)
int order_of_key_upper(const Key& x)
optional<Key> kth(int k)
```

`order_of_key(x)` は `x` 未満の要素数を返す。

`order_of_key_upper(x)` は `x` 以下の要素数を返す。

`kth(k)` は 0-indexed で `k` 番目の要素を返す。それぞれ `x` 以上の最小要素、`x` より大きい最小要素、`x` 以下の最大要素、`x` 未満の最大要素を返す。存在しない場合は `nullopt` を返す。

## 時間計算量

- 期待 $O(\log N)$

## 探索

```cpp
optional<Key> lower_bound(const Key& x)
optional<Key> upper_bound(const Key& x)
optional<Key> max_leq(const Key& x)
optional<Key> max_less(const Key& x)
```

それぞれ `x` 以上の最小要素、`x` より大きい最小要素、`x` 以下の最大要素、`x` 未満の最大要素を返す。存在しない場合は `nullopt` を返す。

## 時間計算量

- 期待 $O(\log N)$

## 状態情報

`size`, `empty`, `clear` は $O(1)$。

## 空間計算量（API 別の追加領域）

- コンストラクタ、`size`、`empty`、`clear`: $O(1)$
- `insert`、`erase`: 期待 $O(\log N)$ の再帰スタック
- `contains`、`order_of_key`、`order_of_key_upper`、`kth`、`lower_bound`、`upper_bound`、`max_leq`、`max_less`: $O(1)$（反復的に探索する）
- ムーブコンストラクタ / ムーブ代入: $O(1)$

ノードは固定長版では容量 $O(\texttt{MAX_SIZE})$ の領域に保存し、`insert` が消費するスロットは 1 個である。削除したスロットは再利用する。

## 注意点

- `Compare` は狭義弱順序でなければならない。同値なキーは 1 個だけ保持する。
- `insert` は既存のキーなら `false` を返し、固定長版で要素数が `MAX_SIZE` の状態で新しいキーを挿入すると `runtime_error` を送出する。
- `erase` は存在しなければ `false` を返す。`kth` と各境界探索は該当なしなら `nullopt` を返す。
- `seed` は優先度の生成だけに使い、同じ `seed` と操作列なら同じ木を構築する。
- `clear` は $O(1)$ でノード領域を再利用する。コピーは禁止、ムーブは可能である。
- 容量を指定した固定配列版（`TreapSet<Key, N>`）と可変長版（`TreapSet<Key>`）は同じヘッダで利用できる。
