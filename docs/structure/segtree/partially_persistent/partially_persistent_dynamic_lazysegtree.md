---
title: Partially Persistent Dynamic Lazy Segment Tree (部分永続動的遅延セグメント木)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_dynamic_lazysegtree.hpp
---

巨大な添字範囲を扱う部分永続遅延セグメント木。更新は最新versionにだけ行い、ノードの値と遅延作用の履歴から過去versionを参照する。

```cpp
PartiallyPersistentDynamicLazySegtree<MonoidActLen, MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>
```

```cpp
int set(long long index, S value)
int apply(long long index, T action)
int apply(long long l, long long r, T action)
S get(int version, long long index)
S prod(int version, long long l, long long r)
S all_prod(int version)
long long max_right(int version, long long l, F f)
long long min_left(int version, long long r, F f)
```

`MAX_NODES`は疎ノード数、`MAX_CHANGES`は値と遅延作用をまとめた変更履歴数の上限。

## 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `set`, 1点`apply`, 区間`apply`: $O(\log \mathtt{MAX\_SIZE})$
- 最新versionの`get`, `prod`, `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionの同query: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- 最新versionの`all_prod`: $O(1)$、過去versionの`all_prod`: $O(\log \mathtt{MAX\_CHANGES})$
- version・node・change情報: $O(1)$

空間計算量は $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$。

## 注意点

version 0が初期状態で、queryは `0 <= version < versions()` の任意versionを参照できる。
更新は `latest_version()` だけを元に行い、新しいversion番号を返す。過去versionを更新元に
指定すると `runtime_error`。点は列内、区間は半開区間 `[l,r)` である。
境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正なversion・点・区間・predicate、version/node/change容量超過では `runtime_error`。
失敗した更新はversion・node・履歴使用量を更新前へ戻す。各APIの計算量は上記表の通り。

## 状態API

```cpp
long long size() const
int versions() const
int latest_version() const
int nodes_used() const
int changes_used() const
```

順に領域長、version数、最新番号、使用node数、変更履歴数を返し、すべて (O(1))。

## 容量を省略する場合

容量引数の既定値は `-1`。`PartiallyPersistentDynamicLazySegtree<Monoid> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
