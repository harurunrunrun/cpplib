---
title: Partially Persistent Dynamic 2D Segment Tree (部分永続動的二次元セグメント木)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_dynamic_segtree_2d.hpp
---

必要な経路だけを生成する部分永続二次元セグメント木。更新は最新 version に対してだけ行い、すべての過去 version を参照できる。完全永続版のラッパーではなく、共有した疎な座標木の各ノードに値の変更履歴を保持する。

```cpp
PartiallyPersistentDynamicSegtree2D<
    Monoid, MAX_HEIGHT, MAX_WIDTH,
    MAX_ROW_NODES, MAX_COLUMN_NODES, MAX_VERSIONS, MAX_CHANGES
> seg;
```

座標領域は $[0, \texttt{MAX_HEIGHT}) \times [0, \texttt{MAX_WIDTH})$ で、未更新の点の値は `Monoid.e()` である。`Monoid.op` は結合的かつ可換でなければならない。version 0 が初期状態で、`MAX_CHANGES` は列方向のノードに記録できる値変更の総数である。

容量（6 つのテンプレート引数）をすべて省略すると可変長版になり、`PartiallyPersistentDynamicSegtree2D<Monoid> tree(height, width);` のように大きさを実行時に与える。ノード・version・履歴は使用量に応じて `std::vector` を拡張する。

## version と使用量

```cpp
int versions() const
int latest_version() const
int row_nodes_used() const
int column_nodes_used() const
int changes_used() const
```

順に version 数、最新 version 番号、行方向・列方向のノードプールの使用ノード数、値履歴の使用数を返す。すべて $O(1)$ である。

## 最新 version の更新

```cpp
int set(long long row, long long col, const S& value)
int apply(long long row, long long col, const S& value)
```

`set` は点の値を置き換え、`apply` は `Monoid.op(current, value)` を格納する。どちらも新しい version 番号を返す。過去の version を指定する更新 API はなく、version の分岐はできない。

## 任意 version の取得

```cpp
S get(int version, long long row, long long col) const
S get(long long row, long long col) const
S prod(int version, long long rb, long long cb, long long re, long long ce) const
S prod(long long rb, long long cb, long long re, long long ce) const
S all_prod(int version) const
S all_prod() const
```

`version` を省略した版は最新 version を参照する。`prod` は半開矩形 $[rb, re) \times [cb, ce)$ の総積を返す。

## 時間計算量

$H = \texttt{MAX_HEIGHT}$, $W = \texttt{MAX_WIDTH}$ とし、$R, C$ を行方向・列方向の更新経路長、$L=\lceil\log_2(\texttt{MAX_CHANGES}+1)\rceil$ とする。

- コンストラクタ: $O(\texttt{MAX_ROW_NODES}+\texttt{MAX_COLUMN_NODES}+\texttt{MAX_CHANGES}\,L)$
- `set`, `apply`: $O(RCL)$。1 回の更新で値履歴をちょうど $RC$ 件追加し、座標木はまだ存在しない経路上のノードだけを生成する。
- `get`: $O(\log H+\log W+L)$
- `prod`: $O(L\log H\log W)$
- `all_prod`: $O(L)$
- version と使用量の取得: $O(1)$

最新 version の履歴参照は先頭の記録から直接返せるが、上の値は任意の version に対する上界である。保存領域は $O(\texttt{MAX_ROW_NODES}+\texttt{MAX_COLUMN_NODES}+\texttt{MAX_CHANGES}\,L)$ である。

## 注意点

- 点・矩形・version の範囲外、逆向きの区間、version・ノード・履歴の容量超過では `std::runtime_error` を送出する。
- 失敗した更新が生成したノードと履歴は取り消され、version 数も変化しない。
- コピーコンストラクタとコピー代入は削除されている。
