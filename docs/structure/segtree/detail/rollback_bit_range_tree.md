---
title: Rollback Bit Range Tree Internal (ロールバック可能ビット区間木内部実装)
documentation_of: ../../../../src/structure/segtree/detail/rollback_bit_range_tree.hpp
---

ビット列に対する区間代入・区間反転・区間和とロールバックを扱う、動的区間木の共通実装である。

## API

```cpp
template<long long MAX_SIZE = -1, int MAX_NODES = -1, int MAX_VERSIONS = -1, int MAX_CHANGES = -1>
struct bit_range_tree_detail::RollbackBitRangeTree;

bit_range_tree_detail::RollbackBitRangeTree<
    MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES
> tree(n);
```

- `size()`: 列の長さを返す。
- `versions()` / `latest_version()` / `snapshot()`: バージョン数、最新のバージョン番号、現在のバージョン番号（`latest_version()` と同じ）を返す。
- `nodes_used()` / `changes_used()`: 使用中のノード数と変更履歴の数を返す。
- `fork()`: 現在の状態と同じ内容の新しいバージョンを作る。
- `assign(l, r, value)`: `[l, r)` を `value`（0 または 1）にする。
- `flip(l, r)`: `[l, r)` を反転する。
- `set_one(l, r)` / `set(l, r)`: `[l, r)` を 1 にする。
- `sum(l, r)`: 現在の `[l, r)` にある 1 の個数を返す。
- `get(k)`: 現在の `k` 番目のビットを返す。
- `rollback(version)`: 指定したバージョンの状態に戻し、それより新しいバージョンを破棄する。

更新操作は更新後のバージョン番号を返す。

## 時間計算量

- コンストラクタ: $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS}+\texttt{MAX_CHANGES})$
- 更新、`sum`, `get`: $O(\log \texttt{MAX_SIZE})$
- `fork`, `snapshot` と使用量の取得: $O(1)$
- `rollback`: 取り消す変更履歴の数を $K$ として $O(K)$

更新 1 回の追加の空間計算量は $O(\log \texttt{MAX_SIZE})$ である。

## 注意点

- 公開ラッパー群の内部部品であり、通常は直接使わない。
- ロールバックで破棄されたバージョン番号は無効になる。
- `0 < n <= MAX_SIZE`、区間は `0 <= l <= r <= n`、点は `[0, n)` を要求する。範囲・バージョンの違反や、ノード・バージョン・変更履歴の容量超過では `std::runtime_error` を送出する。

## 容量を省略する場合

容量のテンプレート引数のデフォルトはすべて `-1` である。`RollbackBitRangeTree<> tree(n);` のようにすべての容量を省略すると、同じヘッダ内の可変長版を使う（一部だけを省略することはできない）。可変長版ではノード・バージョン情報・変更履歴を使用量に応じて `std::vector` で拡張し、更新・クエリのアルゴリズムは固定長版と同じである。上記の計算量の $\texttt{MAX_SIZE}$ は $n$ に置き換わり、コンストラクタの容量に比例する初期化は不要になる。`n` が大きすぎる場合は `std::length_error` を送出する。可変長版でもコンストラクタのデフォルト引数 `n = 0` は `0 < n` の条件に反するので、`n` は必ず指定する。
