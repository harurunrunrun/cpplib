---
title: Persistent Bit Range Tree Internal (完全永続ビット区間木内部実装)
documentation_of: ../../../../src/structure/segtree/detail/persistent_bit_range_tree.hpp
---

ビット列に対する区間代入・区間反転・区間和を扱う、完全永続な動的区間木の共通実装である。

## API

```cpp
template<long long MAX_SIZE = -1, int MAX_NODES = -1, int MAX_VERSIONS = -1>
struct bit_range_tree_detail::PersistentBitRangeTree;

bit_range_tree_detail::PersistentBitRangeTree<
    MAX_SIZE, MAX_NODES, MAX_VERSIONS
> tree(n);
```

- `size()`: 列の長さを返す。
- `versions()` / `latest_version()`: バージョン数と最新のバージョン番号を返す。
- `nodes_used()`: 使用中のノード数を返す。
- `fork(version)`: 指定したバージョンと同じ内容の新しいバージョンを作る。
- `assign(version, l, r, value)`: `[l, r)` を `value`（0 または 1）にした新しいバージョンを作り、その番号を返す。
- `flip(version, l, r)`: `[l, r)` を反転した新しいバージョンを返す。
- `set_one(version, l, r)`: `[l, r)` を 1 にした新しいバージョンを返す。
- `sum(version, l, r)`: 指定したバージョンの `[l, r)` にある 1 の個数を返す。
- `get(version, k)`: 指定したバージョンの `k` 番目のビットを返す。

## 時間計算量

- コンストラクタ: $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS})$
- `assign`, `flip`, `set_one`, `sum`, `get`: $O(\log \texttt{MAX_SIZE})$
- `fork` と使用量の取得: $O(1)$

更新 1 回につき $O(\log \texttt{MAX_SIZE})$ 個のノードを追加する。

## 注意点

- 公開ラッパー群の内部部品であり、通常は直接使わない。
- 任意の有効なバージョンから分岐でき、既存のバージョンは変更されない。
- `0 < n <= MAX_SIZE`、区間は `0 <= l <= r <= n`、点は `[0, n)` を要求する。バージョン・範囲の違反や、ノード・バージョンの容量超過では `std::runtime_error` を送出する。

## 容量を省略する場合

容量のテンプレート引数のデフォルトはすべて `-1` である。`PersistentBitRangeTree<> tree(n);` のようにすべての容量を省略すると、同じヘッダ内の可変長版を使う（一部だけを省略することはできない）。可変長版ではノード・バージョン情報・変更履歴を使用量に応じて `std::vector` で拡張し、更新・クエリのアルゴリズムは固定長版と同じである。上記の計算量の $\texttt{MAX_SIZE}$ は $n$ に置き換わり、コンストラクタの容量に比例する初期化は不要になる。`n` が大きすぎる場合は `std::length_error` を送出する。可変長版では `0 <= n` を要求し、コンストラクタのデフォルト引数 `n = 0` では長さ $0$ の列を作る（区間は空区間 `[0, 0)` のみ有効）。
