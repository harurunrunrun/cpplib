---
title: Persistent Dynamic 2D Segment Tree (完全永続動的二次元セグメント木)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_dynamic_segtree_2d.hpp
---

必要な経路だけを生成する完全永続二次元セグメント木。任意の過去の version から更新を分岐できる。座標領域は $[0, \texttt{MAX_HEIGHT}) \times [0, \texttt{MAX_WIDTH})$ で、未更新の点の値は `Monoid.e()` である。

```cpp
PersistentDynamicSegtree2D<
    Monoid, MAX_HEIGHT, MAX_WIDTH,
    MAX_ROW_NODES, MAX_COLUMN_NODES, MAX_VERSIONS
> seg;
```

`Monoid.op` は結合的かつ可換でなければならない。version 0 が初期状態で、`MAX_VERSIONS` は version 0 を除いて追加できる version 数である。

容量（5 つのテンプレート引数）をすべて省略すると可変長版になり、`PersistentDynamicSegtree2D<Monoid> tree(height, width);` のように大きさを実行時に与える。ノードと version は使用量に応じて `std::vector` を拡張する。

## version と使用量

```cpp
int versions() const
int latest_version() const
int row_nodes_used() const
int column_nodes_used() const
```

`versions` は現在存在する version 数、`latest_version` は最後に追加した version 番号を返す。残りの 2 つは全 version で共有するノードプール（行方向・列方向）の使用ノード数を返す。すべて $O(1)$ である。

## 更新と分岐

```cpp
int set(int version, long long row, long long col, const S& value)
int set(long long row, long long col, const S& value)
int apply(int version, long long row, long long col, const S& value)
int apply(long long row, long long col, const S& value)
int fork(int version)
```

`set` は点の値を置き換え、`apply` は現在の値を `Monoid.op(current, value)` に更新する。`version` を省略した版は最新 version から更新する。どの更新も元の version を変更せず、新しい version 番号を返す。`fork` は指定 version と同じ根を共有する version を追加する。

$R$ を行方向の更新経路長、$C$ を列方向の更新経路長とすると、`set` と `apply` は成功時に行方向のノードをちょうど $R$ 個、列方向のノードをちょうど $RC$ 個複製する。`fork` はノードを生成しない。

## 取得

```cpp
S get(int version, long long row, long long col) const
S get(long long row, long long col) const
S prod(int version, long long rb, long long cb, long long re, long long ce) const
S prod(long long rb, long long cb, long long re, long long ce) const
S all_prod(int version) const
S all_prod() const
```

`version` を省略した版は最新 version を参照する。`get` は 1 点の値、`prod` は半開矩形 $[rb, re) \times [cb, ce)$ の総積、`all_prod` は全領域の総積を返す。

## 時間計算量

$H=\texttt{MAX_HEIGHT}$、$W=\texttt{MAX_WIDTH}$ とする。`version` を省略した版も同じである。

| API | 時間計算量 | 永続保存領域 / 一時領域 |
| --- | --- | --- |
| コンストラクタ | $O(\texttt{MAX_ROW_NODES}+\texttt{MAX_COLUMN_NODES}+\texttt{MAX_VERSIONS})$ | 同量の固定領域 |
| `versions`, `latest_version`, `row_nodes_used`, `column_nodes_used` | $O(1)$ | $O(1)$ |
| `set`, `apply` | $O(\log H\log W)$ | 行ノード $O(\log H)$、列ノード $O(\log H\log W)$、スタック $O(\log H+\log W)$ |
| `fork` | $O(1)$ | ノード追加なし、一時 $O(1)$ |
| `get` | $O(\log H+\log W)$ | スタック $O(\log H+\log W)$ |
| `prod` | $O(\log H\log W)$ | スタック $O(\log H+\log W)$ |
| `all_prod` | $O(1)$ | $O(1)$ |

## 注意点

- 点・矩形・version の範囲外、逆向きの区間、各容量の超過では `std::runtime_error` を送出する。
- 容量不足や値のコピーの失敗で更新が失敗した場合、version 数とノードプールの使用量は更新前に戻る。
- コピーコンストラクタとコピー代入は削除されている。
