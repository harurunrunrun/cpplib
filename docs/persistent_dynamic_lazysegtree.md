---
title: Persistent Dynamic Lazy Segment Tree (完全永続動的遅延セグメント木)
documentation_of: ../src/structure/segtree/persistent_dynamic_lazysegtree.hpp
---

巨大な添字範囲を扱う完全永続遅延セグメント木。

```cpp
PersistentDynamicLazySegtree<MonoidActLen, MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

`MAX_NODES`は全versionで共有するノード数、`MAX_VERSIONS`はversion 0を除くversion数の上限。

```cpp
int set(int version, long long index, S value)
int apply(int version, long long index, T action)
int apply(int version, long long l, long long r, T action)
int fork(int version)
S get(int version, long long index)
S prod(int version, long long l, long long r)
S all_prod(int version)
long long max_right(int version, long long l, F f)
long long min_left(int version, long long r, F f)
```

更新は指定versionから分岐し、新しいversion番号を返す。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `set`, 1点`apply`, 区間`apply`, `get`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `all_prod`, `size`, `versions`, `latest_version`, `nodes_used`: $O(1)$

各更新が複製するノード数は $O(\log \mathtt{MAX\_SIZE})$。

# 共通API契約

version 0が初期状態で、`0 <= version < versions()` のversionだけを参照できる。
更新と `fork` は指定versionを変更せず、新しいversion番号を返す。`latest_version()` は
`versions()-1`。点は実際の列内、区間は半開区間 `[l,r)` で指定する。
境界探索を持つ型ではpredicateは単調かつ単位元に対してtrueでなければならない。

不正なversion・点・区間・predicate、version/node容量超過では `runtime_error`。
容量不足で更新が失敗した場合、version数と使用node数は更新前へ戻る。
各APIの計算量は上記表の通り。
