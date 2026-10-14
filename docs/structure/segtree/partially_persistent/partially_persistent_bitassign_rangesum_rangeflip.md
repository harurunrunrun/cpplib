---
title: Partially Persistent BitAssign RangeSum RangeFlip (部分永続ビット代入・区間和・区間反転)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_bitassign_rangesum_rangeflip.hpp
---

$0,1$ 列の区間更新と区間和を部分永続化する。更新は最新versionだけに行い、過去の全versionを参照できる。

完全永続版は使用せず、各疎segment tree nodeの変更履歴だけを保存する。

```cpp
PartiallyPersistentBitAssignRangeSumRangeFlip<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>
```

`PartiallyPersistentDynamicBitAssignRangeSumRangeFlip` という同じAPIの別名も定義される。

## 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `assign`, `flip`, `set_one`, `set`: $O(\log \mathtt{MAX\_SIZE})$
- 最新versionの`sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionの`sum`, `get`: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- `fork`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$

空間計算量は $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$。

## 注意点

```cpp
int assign(int version, long long l, long long r, bool value)
int flip(int version, long long l, long long r)
int set_one(int version, long long l, long long r)
long long sum(int version, long long l, long long r) const
bool get(int version, long long k) const
int fork(int version)
long long size() const
int versions() const
int latest_version() const
int nodes_used() const
int changes_used() const
```

version省略updateは最新版を使う。更新元は `latest_version()` に限り、新version番号を返す。
queryは任意の有効versionを参照する。`set` は `set_one` と同じ。
区間は半開区間。有効version・範囲・最新版条件・各容量の違反では `runtime_error`。
失敗した更新は使用量を元へ戻す。各APIの計算量は下表の通り。

## 容量を省略する場合

容量引数の既定値は `-1`。`PartiallyPersistentBitAssignRangeSumRangeFlip<> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
