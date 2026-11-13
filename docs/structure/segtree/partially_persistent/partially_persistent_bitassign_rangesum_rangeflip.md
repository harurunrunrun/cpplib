---
title: Partially Persistent BitAssign RangeSum RangeFlip (部分永続ビット代入・区間和・区間反転)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_bitassign_rangesum_rangeflip.hpp
---

0/1 の列に対する区間代入・区間反転と区間和を部分永続化する。更新は最新のバージョンにだけ行い、過去のすべてのバージョンを参照できる。内部実装は `bit_range_tree_detail::PartiallyPersistentBitRangeTree`（部分永続ビット区間木内部実装）を継承したもので、各疎セグメント木ノードの変更履歴だけを保存する。

```cpp
template<long long MAX_SIZE = -1, int MAX_NODES = -1, int MAX_VERSIONS = -1, int MAX_CHANGES = -1>
struct PartiallyPersistentBitAssignRangeSumRangeFlip;
```

`PartiallyPersistentDynamicBitAssignRangeSumRangeFlip` という同じ API の別名も定義される。`MAX_SIZE` は列の長さの上限、`MAX_NODES` はノード数、`MAX_VERSIONS` はバージョン数、`MAX_CHANGES` は変更履歴の数の上限である。

## API

```cpp
explicit PartiallyPersistentBitAssignRangeSumRangeFlip(long long n = MAX_SIZE)
int assign(int version, long long l, long long r, bool value)
int assign(long long l, long long r, bool value)
int flip(int version, long long l, long long r)
int flip(long long l, long long r)
int set_one(int version, long long l, long long r)
int set_one(long long l, long long r)
int set(int version, long long l, long long r)
int set(long long l, long long r)
long long sum(int version, long long l, long long r) const
bool get(int version, long long k) const
int fork(int version)
long long size() const
int versions() const
int latest_version() const
int nodes_used() const
int changes_used() const
```

- コンストラクタは長さ `n` の全要素 0 の列をバージョン 0 として作る。
- 更新（`assign`, `flip`, `set_one`, `set`）は最新版の半開区間 `[l, r)` を書き換えた新しいバージョンを作り、その番号を返す。`version` を省略した形は最新版を更新元とする。`set` は `set_one` と同じで、区間を 1 にする。
- `fork(version)` は最新版と同じ内容の新しいバージョンを作る。
- `sum(version, l, r)` は任意の有効なバージョンの `[l, r)` にある 1 の個数を、`get(version, k)` は `k` 番目のビットを返す。
- `size`, `versions`, `latest_version`, `nodes_used`, `changes_used` は列の長さ・バージョン数・最新のバージョン番号・使用中のノード数・変更履歴の数を返す。

## 時間計算量

- コンストラクタ: $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS}+\texttt{MAX_CHANGES})$
- `assign`, `flip`, `set_one`, `set`: $O(\log \texttt{MAX_SIZE})$
- 最新版の `sum`, `get`: $O(\log \texttt{MAX_SIZE})$
- 過去のバージョンの `sum`, `get`: $O(\log \texttt{MAX_SIZE}\log \texttt{MAX_CHANGES})$
- `fork`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$

空間計算量は $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS}+\texttt{MAX_CHANGES})$ である。更新 1 回で $O(\log \texttt{MAX_SIZE})$ 個のノード・変更履歴を使い、クエリは同じ量の再帰スタックを使う。

## 注意点

- 更新元は常に `latest_version()` でなければならない。バージョン引数付きの更新や `fork` も過去のバージョンは受け付けない。
- `0 < n <= MAX_SIZE` を要求する。区間は半開区間で指定する。
- 無効なバージョン、範囲外、最新版の条件の違反、ノード・バージョン・変更履歴の容量超過では `std::runtime_error` を送出する。失敗した更新は使用量を元に戻す。

## 容量を省略する場合

容量のテンプレート引数のデフォルトはすべて `-1` である。`PartiallyPersistentBitAssignRangeSumRangeFlip<> tree(n);` のようにすべての容量を省略すると、内部実装の可変長版を使い、ノード・バージョン情報・変更履歴を使用量に応じて `std::vector` で拡張する。上記の計算量の容量は実際の使用量に置き換わる。`n` は必ず指定する（デフォルトの `n = 0` は `0 < n` の条件に反する）。
