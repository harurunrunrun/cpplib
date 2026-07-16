---
title: Persistent Lazy Red Black Tree (完全永続遅延赤黒木)
documentation_of: ../src/structure/bbst/persistent_lazy_red_black_tree.hpp
---

path-copy left-leaning red-black tree による完全永続構造。

永続平衡二分探索木によるキー付き列。

キーの半開区間 `[l,r)` に対して、値の作用、値の積を行う。更新は元の version を変更せず、新しい version を返す。

## テンプレート引数

```cpp
PersistentLazyRedBlackTree<Key, MonoidActLen, MAX_NODE, MAX_VERSION, Compare>
```

- キーの型 `Key`
- 作用付きモノイド `MonoidActLen`
- 確保するノード数の上限 `MAX_NODE`
- 保持する version 数の上限 `MAX_VERSION`
- 比較関数 `Compare`

`MonoidActLen` は次を持つ。

```cpp
S op(S a, long long len_a, S b, long long len_b)
S e()
S mapping(T f, S x, long long len)
T composition(T f, T g)
T id()
```

version `0` は空列。

## insert / erase / contains

```cpp
int insert(const Key& key, const S& value, int version = 0)
int erase(const Key& key, int version = 0)
bool contains(const Key& key, int version = 0)
```

`insert` と `erase` は更新後の version を返す。

同じキーの重複は保持しない。`insert` で同じキーが存在する場合、値は変更せずに新しい version を返す。

## get / set

```cpp
optional<S> get(const Key& key, int version = 0)
int set(const Key& key, const S& value, int version = 0)
```

`get` はキーが存在しない場合 `nullopt` を返す。

`set` は更新後の version を返す。キーが存在しない場合、内容は変更しない。

## 区間操作

```cpp
int apply(const Key& l, const Key& r, const T& f, int version = 0)
S prod(const Key& l, const Key& r, int version = 0)
int all_apply(const T& f, int version = 0)
S all_prod(int version = 0)
```

キーの半開区間 `[l,r)` を扱う。

`apply` と `all_apply` は更新後の version を返す。

## 順序

```cpp
int order_of_key(const Key& key, int version = 0)
optional<Key> kth_key(int k, int version = 0)
vector<pair<Key, S>> to_vector(int version = 0)
```

`order_of_key` は `key` より小さいキーの個数を返す。

`kth_key` は 0-indexed で `k` 番目のキーを返す。

`to_vector` は `(key, value)` をキー昇順で返す。

## API別の時間計算量

$N$ を対象versionの要素数とし、比較・モノイド演算・作用を $O(1)$ とする。

- `PersistentLazyRedBlackTree(compare)`: $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- `versions`, `size`, `empty`, `all_prod`: $O(1)$
- `contains`, `get`, `prod`, `order_of_key`, `kth_key`: worst-case $O(\log(N+1))$
- `insert`, `erase`, `set`, `apply`: worst-case $O(\log(N+1))$、新規version $O(1)$、path-copy node $O(\log(N+1))$
- `all_apply`: $O(1)$、新規version $O(1)$、非空なら新規node $O(1)$
- `to_vector`: 出力要素数 $N$ に対して $O(N)$

内容が変わらない更新も新しいversionを返す。探索だけで終了する場合や空区間ではnodeを複製しないことがある。

## 空間計算量

- 固定arenaとroot table: $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- `to_vector` の戻り値と再帰stack: $O(N)$

## 注意点

- `Compare`はstrict weak ordering、versionは`[0,versions())`。違反時は例外。
- キー区間は`Compare(r,l)==false`を要求し、逆転時は例外。
- updateでversion/node容量を超える場合は例外で、既存versionとnode数を変えない。
- updateは内容が変わらない場合もversionを1個作り、queryは容量を消費しない。
- `get`, `kth_key`は該当なしなら`nullopt`。
