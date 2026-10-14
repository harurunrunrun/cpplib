---
title: Persistent Bit Range Tree Internal (完全永続ビット区間木内部実装)
documentation_of: ../../../../src/structure/segtree/detail/persistent_bit_range_tree.hpp
---

bit列の区間代入・反転・区間和を扱う完全永続動的区間木の共通実装。

## API

```cpp
bit_range_tree_detail::PersistentBitRangeTree<
    MAX_SIZE, MAX_NODES, MAX_VERSIONS
> tree(n);
```

- `size()`：列長を返す。
- `versions()` / `latest_version()`：version数と最新versionを返す。
- `nodes_used()`：使用中のnode数を返す。
- `fork(version)`：指定versionと同内容の新versionを作る。
- `assign(version,l,r,value)`：`[l,r)` を0または1へ代入した新versionを返す。
- `flip(version,l,r)`：`[l,r)` を反転した新versionを返す。
- `set_one(version,l,r)`：`[l,r)` を1にした新versionを返す。
- `sum(version,l,r)`：指定versionの `[l,r)` にある1の個数を返す。
- `get(version,k)`：指定versionの `k` 番目を返す。

## 時間計算量

- constructor：$O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `assign`, `flip`, `set_one`, `sum`, `get`：$O(\log \mathtt{MAX\_SIZE})$
- `fork`と使用量取得：$O(1)$

更新1回につき $O(\log \mathtt{MAX\_SIZE})$ nodeを追加する。

## 注意点

- 公開ラッパー群の内部部品であり、通常は直接使用しない。
- 任意の有効versionから分岐でき、既存versionは変更されない。
- `0 < n <= MAX_SIZE`、区間は `0 <= l <= r <= n`、点は `[0,n)`。version、範囲、node・version容量の違反では `runtime_error`。

## 容量を省略する場合

容量引数の既定値は `-1`。`PersistentBitRangeTree<> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
