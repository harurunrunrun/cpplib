---
title: Persistent Dynamic 2D Segment Tree (完全永続動的二次元セグメント木)
documentation_of: ../src/structure/segtree/persistent_dynamic_segtree_2d.hpp
---

必要な経路だけを生成する、固定容量の完全永続二次元segment tree。
任意の過去versionから更新を分岐できる。座標領域は
`[0, MAX_HEIGHT) x [0, MAX_WIDTH)`、未更新点の値は `Monoid.e()` である。

```cpp
PersistentDynamicSegtree2D<
    Monoid, MAX_HEIGHT, MAX_WIDTH,
    MAX_ROW_NODES, MAX_COLUMN_NODES, MAX_VERSIONS
> seg;
```

`Monoid.op` は結合的かつ可換でなければならない。version 0が初期状態で、
`MAX_VERSIONS` はversion 0を除いて追加できるversion数である。

## Versionと使用量

```cpp
int versions() const
int latest_version() const
int row_nodes_used() const
int column_nodes_used() const
```

`versions` は現在存在するversion数、`latest_version` は最後に追加したversion番号を返す。
残り2つは全versionで共有する固定poolの使用node数を返す。すべて $O(1)$。

## 更新と分岐

```cpp
int set(int version, long long row, long long col, const S& value)
int set(long long row, long long col, const S& value)
int apply(int version, long long row, long long col, const S& value)
int apply(long long row, long long col, const S& value)
int fork(int version)
```

`set` は点を置換し、`apply` は現在値を `Monoid.op(current, value)` へ更新する。
version引数なしのoverloadは最新versionから更新する。どの更新も元versionを変更せず、
新しいversion番号を返す。`fork` は指定versionと同じ根を共有するversionを追加する。

$R$ をrow方向の更新経路長、$C$ をcolumn方向の更新経路長とすると、
`set` と `apply` は $O(RC)=O(\log H\log W)$ 時間で、成功時にrow nodeを
ちょうど $R$ 個、column nodeをちょうど $RC$ 個複製する。`fork` は $O(1)$ でnodeを生成しない。

## 取得

```cpp
S get(int version, long long row, long long col) const
S get(long long row, long long col) const
S prod(int version, long long rb, long long cb, long long re, long long ce) const
S prod(long long rb, long long cb, long long re, long long ce) const
S all_prod(int version) const
S all_prod() const
```

version引数なしのoverloadは最新versionを参照する。`get` は1点を
$O(\log H+\log W)$、`prod` は半開矩形 `[rb,re) x [cb,ce)` の積を
$O(\log H\log W)$、`all_prod` は全領域の積を $O(1)$ で返す。

## 注意点

構築は $O(MAX\_ROW\_NODES+MAX\_COLUMN\_NODES+MAX\_VERSIONS)$、
保存領域も同量。点・矩形・versionの範囲違反、逆区間、各固定容量の超過は
`runtime_error`。容量不足や値のコピーで更新が失敗した場合、version数とpool使用量は
更新前へ戻る。コピー構築・コピー代入は禁止している。

## API別の時間計算量

| API | 時間 | 永続保存領域 / 一時領域 |
| --- | --- | --- |
| constructor | $O(MAX\_ROW\_NODES+MAX\_COLUMN\_NODES+MAX\_VERSIONS)$ | 同量の固定領域 |
| `versions`, `latest_version`, `row_nodes_used`, `column_nodes_used` | $O(1)$ | $O(1)$ |
| `set`, `apply` | $O(\log H\log W)$ | row node $O(\log H)$、column node $O(\log H\log W)$、stack $O(\log H+\log W)$ |
| `fork` | $O(1)$ | node追加なし、一時 $O(1)$ |
| `get` | $O(\log H+\log W)$ | stack $O(\log H+\log W)$ |
| `prod` | $O(\log H\log W)$ | stack $O(\log H+\log W)$ |
| `all_prod` | $O(1)$ | $O(1)$ |

$H=\mathtt{MAX\_HEIGHT}$、$W=\mathtt{MAX\_WIDTH}$。version省略overloadも同じ。
