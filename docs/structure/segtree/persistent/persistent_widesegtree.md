---
title: Persistent Wide Segtree (完全永続ワイドセグメント木)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_widesegtree.hpp
---

`long long`の和を扱う完全永続な疎セグメント木。

```cpp
PersistentWideSegtree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

```cpp
int set(int version, long long index, long long value)
int add(int version, long long index, long long value)
int fork(int version)
long long get(int version, long long index)
long long prod(int version, long long l, long long r)
long long all_prod(int version)
```

## 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `set`, `add`, `get`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `all_prod`, `versions`, `latest_version`, `nodes_used`: $O(1)$

## 注意点

version 0が初期状態で、`0 <= version < versions()` のversionだけを参照できる。
更新と `fork` は指定versionを変更せず、新しいversion番号を返す。`latest_version()` は
`versions()-1`。点は実際の列内、区間は半開区間 `[l,r)` で指定する。
境界探索を持つ型ではpredicateは単調かつ単位元に対してtrueでなければならない。

不正なversion・点・区間・predicate、version/node容量超過では `runtime_error`。
容量不足で更新が失敗した場合、version数と使用node数は更新前へ戻る。
各APIの計算量は上記表の通り。

## 容量を省略する場合

容量引数の既定値は `-1`。`PersistentWideSegtree<> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
