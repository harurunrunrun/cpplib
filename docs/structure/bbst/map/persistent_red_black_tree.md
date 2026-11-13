---
title: Persistent Red Black Tree (完全永続赤黒木)
documentation_of: ../../../../src/structure/bbst/map/persistent_red_black_tree.hpp
---

経路コピーを行う左傾赤黒木による、完全永続な順序付き集合。

同じキーは 1 個だけ保持する。更新操作は新しいバージョンを作ってその番号を返す。バージョン 0 は空集合である。

## テンプレート引数

```cpp
PersistentRedBlackSet<Key, MAX_NODE = -1, MAX_VERSION = -1, Compare = std::less<Key>>
```

- `Key`: キーの型
- `MAX_NODE`: 確保できるノード数の上限
- `MAX_VERSION`: バージョン数（バージョン 0 を含む）の上限
- `Compare`: 比較関数

`MAX_NODE`, `MAX_VERSION` を指定するとノードと根の表を `std::array` でオブジェクト内に固定確保する。省略（`-1`）すると `std::vector` で必要に応じて確保する可変長版になる（再確保のコストは償却され、各操作の計算量は変わらない）。

## コンストラクタ・状態

```cpp
explicit PersistentRedBlackSet(const Compare& compare = Compare())
int versions() const
int size(int version = 0) const
bool empty(int version = 0) const
```

`versions` は現在のバージョン数を返す。`size`, `empty` はバージョン `version` の要素数・空かどうかを返す。

## insert / erase / contains

```cpp
int insert(const Key& x, int version = 0)
int erase(const Key& x, int version = 0)
bool contains(const Key& x, int version = 0) const
```

`insert` と `erase` はバージョン `version` を更新した新しいバージョンを作り、その番号を返す。

## 順序

```cpp
int order_of_key(const Key& x, int version = 0) const
int order_of_key_upper(const Key& x, int version = 0) const
std::optional<Key> kth(int k, int version = 0) const
```

順に、`x` 未満の要素数、`x` 以下の要素数、0-indexed で `k` 番目の要素を返す。

## 探索

```cpp
std::optional<Key> lower_bound(const Key& x, int version = 0) const
std::optional<Key> upper_bound(const Key& x, int version = 0) const
std::optional<Key> max_leq(const Key& x, int version = 0) const
std::optional<Key> max_less(const Key& x, int version = 0) const
std::vector<Key> to_vector(int version = 0) const
```

順に、`x` 以上の最小要素、`x` より大きい最小要素、`x` 以下の最大要素、`x` 未満の最大要素を返す。`to_vector` は全要素を昇順に返す。

## 時間計算量

$N$ を対象バージョンの要素数とし、比較と `Key` のコピーを $O(1)$ とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$、可変長版は $O(1)$
- `versions`, `size`, `empty`: $O(1)$
- `contains`, `order_of_key`, `order_of_key_upper`, `kth`, `lower_bound`, `upper_bound`, `max_leq`, `max_less`: 最悪 $O(\log(N+1))$
- `insert`, `erase`: 最悪 $O(\log(N+1))$。新しいバージョンを 1 個、経路コピーのノードを $O(\log(N+1))$ 個作る
- `to_vector`: $O(N)$

既に存在するキーの `insert`、存在しないキーの `erase` も探索後に新しいバージョンを作るが、ノードは複製しない。

## 空間計算量

- 固定長版のノードと根の表: $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$
- 可変長版: 作ったノード数とバージョン数に比例
- `to_vector` の戻り値とスタック: $O(N)$

## 注意点

- `Compare` は狭義弱順序でなければならない。バージョンは $[0,\texttt{versions()})$ でなければならず、違反すると `std::runtime_error` を投げる。
- 更新でバージョン数・ノード数の上限を超える場合は `std::runtime_error` を投げ、既存のバージョンとノード数は変えない。
- 内容が変わらない更新もバージョンを 1 個作る。
- `kth` と境界探索は該当なしなら `std::nullopt` を返す。
- 固定長版はノード配列をオブジェクト内に持つので、大きな容量ではグローバル変数や `static` 変数として置く。
