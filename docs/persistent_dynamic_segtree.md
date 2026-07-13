---
title: Persistent Dynamic Segment Tree (完全永続動的セグメント木)
documentation_of: ../src/structure/segtree/persistent_dynamic_segtree.hpp
---

巨大な添字範囲を扱う完全永続セグメント木。更新ごとに根から更新点までを複製する。

```cpp
PersistentDynamicSegtree<Monoid, MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

- `MAX_SIZE`: 添字範囲 `[0, MAX_SIZE)`
- `MAX_NODES`: 全versionで共有するノード数の上限
- `MAX_VERSIONS`: version 0を除くversion数の上限

## 更新

```cpp
int set(int version, long long index, S value)
int fork(int version)
```

新しいversion番号を返す。`set`は指定versionから分岐する。初期versionは0。

## 取得

```cpp
S get(int version, long long index)
S prod(int version, long long l, long long r)
S all_prod(int version)
long long max_right(int version, long long l, F f)
long long min_left(int version, long long r, F f)
```

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `set`, `get`, `prod`, `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `all_prod`, `versions`, `latest_version`, `nodes_used`: $O(1)$

`set`が複製するノード数は $O(\log \mathtt{MAX\_SIZE})$。

# 共通API契約

version 0が初期状態で、`0 <= version < versions()` のversionだけを参照できる。
更新と `fork` は指定versionを変更せず、新しいversion番号を返す。`latest_version()` は
`versions()-1`。点は実際の列内、区間は半開区間 `[l,r)` で指定する。
境界探索を持つ型ではpredicateは単調かつ単位元に対してtrueでなければならない。

不正なversion・点・区間・predicate、version/node容量超過では `runtime_error`。
容量不足で更新が失敗した場合、version数と使用node数は更新前へ戻る。
各APIの計算量は上記表の通り。
