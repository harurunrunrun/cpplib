---
title: Persistent BitAssign RangeSum RangeFlip (完全永続ビット代入・区間和・区間反転)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_bitassign_rangesum_rangeflip.hpp
---

$0,1$ 列の区間更新と区間和を、path copyした暗黙segment treeで完全永続化する。

```cpp
PersistentBitAssignRangeSumRangeFlip<MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

`PersistentDynamicBitAssignRangeSumRangeFlip` という同じAPIの別名も定義される。
完全永続版は疎なpath-copy表現を使う。

```cpp
int assign(int version, long long l, long long r, bool value)
int flip(int version, long long l, long long r)
int set_one(int version, long long l, long long r)
int fork(int version)
long long sum(int version, long long l, long long r)
bool get(int version, long long position)
```

更新は新しいversionを返す。`MAX_VERSIONS` はversion 0を含む総version数。

## 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `assign`, `flip`, `set_one`: $O(\log \mathtt{MAX\_SIZE})$
- `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `size`, `versions`, `latest_version`, `nodes_used`: $O(1)$

## 注意点

```cpp
long long size() const
int versions() const
int latest_version() const
int nodes_used() const
int fork(int version)
int assign(int version, long long l, long long r, bool value)
int flip(int version, long long l, long long r)
int set_one(int version, long long l, long long r)
long long sum(int version, long long l, long long r) const
bool get(int version, long long k) const
```

`assign/flip/set_one` は指定versionの半開区間 `[l,r)` を更新した新versionを返す。
`fork` は内容を変えずに分岐する。`sum/get` は指定versionの1の個数・1点を返す。

`0 < n <= MAX_SIZE`、有効version、`0 <= l <= r <= n`、`0 <= k < n` が必要。
範囲・version・node/version容量違反は `runtime_error`。失敗時に使用量は増えない。
各APIの計算量は下表の通り。

## 容量を省略する場合

容量引数の既定値は `-1`。`PersistentBitAssignRangeSumRangeFlip<> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
