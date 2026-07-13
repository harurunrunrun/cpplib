---
title: Persistent Wide Segtree (完全永続ワイドセグメント木)
documentation_of: ../src/structure/segtree/persistent_widesegtree.hpp
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

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `set`, `add`, `get`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `all_prod`, `versions`, `latest_version`, `nodes_used`: $O(1)$

# 共通API契約

version 0が初期状態で、`0 <= version < versions()` のversionだけを参照できる。
更新と `fork` は指定versionを変更せず、新しいversion番号を返す。`latest_version()` は
`versions()-1`。点は実際の列内、区間は半開区間 `[l,r)` で指定する。
境界探索を持つ型ではpredicateは単調かつ単位元に対してtrueでなければならない。

不正なversion・点・区間・predicate、version/node容量超過では `runtime_error`。
容量不足で更新が失敗した場合、version数と使用node数は更新前へ戻る。
各APIの計算量は上記表の通り。
