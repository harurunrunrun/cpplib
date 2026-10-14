---
title: Persistent Segment Tree Beats (完全永続セグメント木Beats)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_segtree_beats.hpp
---

完全永続 Segment Tree Beats。更新で変更するnodeだけをpath copyし、任意versionから分岐できる。

```cpp
PersistentSegmentTreeBeats<T, MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

`MAX_NODES` は初期木と全更新で作るnode数の合計上限。`MAX_VERSIONS` はversion 0を除いて追加できるversion数の上限。

```cpp
int range_chmin(int version, int l, int r, const T& x)
int range_chmax(int version, int l, int r, const T& x)
int range_add(int version, int l, int r, const T& x)
int set(int version, int k, const T& x)
int fork(int version)
```

新しいversion番号を返す。容量不足時は例外を送出し、node数とversion数を更新前へ戻す。

```cpp
T get(int version, int k) const
T range_sum(int version, int l, int r) const
T range_min(int version, int l, int r) const
T range_max(int version, int l, int r) const
T all_sum(int version) const
T all_min(int version) const
T all_max(int version) const
```

queryは木を書き換えない。祖先nodeに残る加算・上限制約・下限制約を子へ順に合成して評価する。

```cpp
int size() const
int versions() const
int latest_version() const
int nodes_used() const
```

区間は0-indexed半開区間。

## 時間計算量

- constructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `range_chmin`, `range_chmax`: amortized $O(\log^2 N)$
- `range_add`, `set`, `get`, `range_sum`, `range_min`, `range_max`: $O(\log N)$
- `fork`, `all_sum`, `all_min`, `all_max`, version・node情報: $O(1)$

空間計算量は $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$。

## 注意点

version 0が初期状態で、`0 <= version < versions()` のversionだけを参照できる。
更新と `fork` は指定versionを変更せず、新しいversion番号を返す。`latest_version()` は
`versions()-1`。点は実際の列内、区間は半開区間 `[l,r)` で指定する。
境界探索を持つ型ではpredicateは単調かつ単位元に対してtrueでなければならない。

不正なversion・点・区間・predicate、version/node容量超過では `runtime_error`。
容量不足で更新が失敗した場合、version数と使用node数は更新前へ戻る。
各APIの計算量は上記表の通り。

## 容量を省略する場合

容量引数の既定値は `-1`。`PersistentSegmentTreeBeats<T> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
