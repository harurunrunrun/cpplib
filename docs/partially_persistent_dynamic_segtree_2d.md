---
title: Partially Persistent Dynamic 2D Segment Tree (部分永続動的二次元セグメント木)
documentation_of: ../src/structure/segtree/partially_persistent_dynamic_segtree_2d.hpp
---

必要な経路だけを生成する、固定容量の部分永続二次元segment tree。
更新は最新versionにだけ行い、すべての過去versionを参照できる。
完全永続版のwrapperではなく、共有した疎な座標木に値の変更履歴を保持する。

```cpp
PartiallyPersistentDynamicSegtree2D<
    Monoid, MAX_HEIGHT, MAX_WIDTH,
    MAX_ROW_NODES, MAX_COLUMN_NODES, MAX_VERSIONS, MAX_CHANGES
> seg;
```

座標領域は `[0, MAX_HEIGHT) x [0, MAX_WIDTH)`、未更新点の値は `Monoid.e()`。
`Monoid.op` は結合的かつ可換でなければならない。version 0が初期状態で、
`MAX_CHANGES` はcolumn nodeへ記録できる値変更の総数である。

## Versionと使用量

```cpp
int versions() const
int latest_version() const
int row_nodes_used() const
int column_nodes_used() const
int changes_used() const
```

順にversion数、最新version、row/column poolの使用node数、値履歴の使用数を返す。
すべて $O(1)$。

## 最新versionの更新

```cpp
int set(long long row, long long col, const S& value)
int apply(long long row, long long col, const S& value)
```

`set` は点を置換し、`apply` は `Monoid.op(current, value)` を格納する。
新しいversion番号を返す。過去versionを指定する更新APIはなく、version分岐はできない。

$R,C$ をrow/columnの更新経路長、$L=\lceil\log_2(MAX\_CHANGES+1)\rceil$
とすると、時間計算量は $O(RCL)$。1更新につき値履歴をちょうど $RC$ 件追加し、
座標木についてはまだ存在しない経路nodeだけを生成する。

## 過去versionの取得

```cpp
S get(int version, long long row, long long col) const
S get(long long row, long long col) const
S prod(int version, long long rb, long long cb, long long re, long long ce) const
S prod(long long rb, long long cb, long long re, long long ce) const
S all_prod(int version) const
S all_prod() const
```

version引数なしのoverloadは最新versionを参照する。`get` は
$O(\log H+\log W+L)$、半開矩形を返す `prod` は
$O(\log H\log W L)$、`all_prod` は $O(L)$。最新versionの履歴参照は先頭記録から
直接返せるが、表では任意versionの上界を示している。

## 注意点

構築時間と保存領域は
$O(MAX\_ROW\_NODES+MAX\_COLUMN\_NODES+MAX\_CHANGES\,L)$。
点・矩形・versionの範囲違反、逆区間、version/node/history容量超過は
`runtime_error`。失敗した更新が生成したnodeと履歴は取り消され、version数も変化しない。
コピー構築・コピー代入は禁止している。
