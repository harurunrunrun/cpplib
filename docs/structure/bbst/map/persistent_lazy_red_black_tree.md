---
title: Persistent Lazy Red Black Tree (完全永続遅延赤黒木)
documentation_of: ../../../../src/structure/bbst/map/persistent_lazy_red_black_tree.hpp
---

経路コピーを行う左傾赤黒木による、完全永続なキー付きの列。

キーの半開区間 $[l,r)$ に対して、値への作用と値の積を扱う。更新は元のバージョンを変更せず、新しいバージョンを作ってその番号を返す。

## テンプレート引数

```cpp
PersistentLazyRedBlackTree<Key, MonoidActLen, MAX_NODE = -1, MAX_VERSION = -1, Compare = std::less<Key>>
```

- `Key`: キーの型
- `MonoidActLen`: 作用付きモノイド（区間長付き）のオブジェクト
- `MAX_NODE`: 確保できるノード数の上限
- `MAX_VERSION`: バージョン数（バージョン 0 を含む）の上限
- `Compare`: 比較関数

`MAX_NODE`, `MAX_VERSION` を指定するとノードと根の表をオブジェクト内に固定確保する。省略（`-1`）すると `std::vector` で必要に応じて確保する可変長版になる（再確保のコストは償却され、各操作の計算量は変わらない）。

`MonoidActLen` は次を持つ。

```cpp
S op(S a, long long len_a, S b, long long len_b)
S e()
S mapping(T f, S x, long long len)
T composition(T f, T g)
T id()
```

バージョン 0 は空の列である。`versions()`, `size(version = 0)`, `empty(version = 0)` でバージョン数・要素数・空かどうかを取得できる。

## insert / erase / contains

```cpp
int insert(const Key& key, const S& value, int version = 0)
int erase(const Key& key, int version = 0)
bool contains(const Key& key, int version = 0) const
```

`insert` と `erase` は更新後のバージョンを返す。

同じキーの重複は保持しない。`insert` で同じキーが存在する場合、値は変更せずに新しいバージョンを返す。

## get / set

```cpp
std::optional<S> get(const Key& key, int version = 0) const
int set(const Key& key, const S& value, int version = 0)
```

`get` はキーが存在しない場合 `nullopt` を返す。

`set` は更新後のバージョンを返す。キーが存在しない場合、内容は変更しない。

## 区間操作

```cpp
int apply(const Key& l, const Key& r, const T& f, int version = 0)
S prod(const Key& l, const Key& r, int version = 0) const
int all_apply(const T& f, int version = 0)
S all_prod(int version = 0) const
```

キーの半開区間 `[l,r)` を扱う。

`apply` と `all_apply` は更新後のバージョンを返す。

## 順序

```cpp
int order_of_key(const Key& key, int version = 0) const
std::optional<Key> kth_key(int k, int version = 0) const
std::vector<std::pair<Key, S>> to_vector(int version = 0) const
```

`order_of_key` は `key` より小さいキーの個数を返す。

`kth_key` は 0-indexed で `k` 番目のキーを返す。

`to_vector` は `(key, value)` をキー昇順で返す。

## 時間計算量

$N$ を対象バージョンの要素数とし、比較・モノイド演算・作用を $O(1)$ とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$、可変長版は $O(1)$
- `versions`, `size`, `empty`, `all_prod`: $O(1)$
- `contains`, `get`, `prod`, `order_of_key`, `kth_key`: 最悪 $O(\log(N+1))$
- `insert`, `erase`, `set`, `apply`: 最悪 $O(\log(N+1))$、新規バージョン $O(1)$、経路コピーノード $O(\log(N+1))$
- `all_apply`: $O(1)$、新規バージョン $O(1)$、非空なら新規ノード $O(1)$
- `to_vector`: 出力要素数 $N$ に対して $O(N)$

内容が変わらない更新も新しいバージョンを返す。探索だけで終了する場合や空区間ではノードを複製しないことがある。

## 空間計算量

- 固定長版のノードと根の表: $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$。可変長版は作ったノード数とバージョン数に比例
- `to_vector` の戻り値と再帰スタック: $O(N)$

## 注意点

- `Compare` は狭義弱順序、バージョンは $[0,\texttt{versions()})$。違反すると `std::runtime_error` を投げる。
- キー区間が空または逆転している（`Compare(l, r)` が偽）場合、`apply` は内容を変えずに新しいバージョンを作り、`prod` は単位元を返す。例外は投げない。
- 更新でバージョン/ノード容量を超える場合は `std::runtime_error` を投げ、既存バージョンとノード数を変えない。
- 更新は内容が変わらない場合もバージョンを 1 個作り、クエリは容量を消費しない。
- `get`, `kth_key` は該当なしなら `nullopt`。
