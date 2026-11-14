---
title: Persistent Dynamic Lazy Segment Tree (完全永続動的遅延セグメント木)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_dynamic_lazysegtree.hpp
---

巨大な添字範囲を扱う完全永続遅延セグメント木。更新ごとに必要なノードを複製し、任意の version から分岐できる。

```cpp
PersistentDynamicLazySegtree<Monoid, MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

`Monoid` は区間長付きの作用モノイド（`MonoidActLen` 等）のオブジェクトである。

- `MAX_SIZE`: 添字範囲の長さの上限
- `MAX_NODES`: 全 version で共有するノード数の上限
- `MAX_VERSIONS`: version 0 を除く version 数の上限

容量（3 つのテンプレート引数）をすべて省略すると可変長版になり、`PersistentDynamicLazySegtree<Monoid> tree(n);` のように長さを実行時に与える。ノードと version は使用量に応じて `std::vector` を拡張する。

## API

```cpp
explicit PersistentDynamicLazySegtree(long long n = MAX_SIZE)  // 可変長版の既定値は 0
int set(int version, long long index, S value)
int apply(int version, long long index, T action)
int apply(int version, long long l, long long r, T action)
int fork(int version)
S get(int version, long long index) const
S prod(int version, long long l, long long r) const
S all_prod(int version) const
long long max_right(int version, long long l, F f, Args... args) const
long long min_left(int version, long long r, F f, Args... args) const
long long size() const
int versions() const
int latest_version() const
int nodes_used() const
```

- コンストラクタ: 長さ $n$（$0 \le n \le \texttt{MAX_SIZE}$）の範囲を全要素 `e()` として version 0 を作る。
- `set`, `apply`, `fork`: 指定 version から分岐した新しい version を作り、その番号を返す。空区間への `apply` は `fork` と同じである。
- `get`, `prod`, `all_prod`, `max_right`, `min_left`: 指定 version を参照する。述語は追加引数 `args...` を受け取れる。

## 時間計算量

$N = \texttt{MAX_SIZE}$（可変長版では $n$）とする。

- コンストラクタ: 固定容量版は $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS})$
- `set`, 1 点 `apply`, 区間 `apply`, `get`, `prod`: $O(\log N)$
- `max_right`, `min_left`: $O(\log N)$
- `fork`, `all_prod`, `size`, `versions`, `latest_version`, `nodes_used`: $O(1)$

各更新で複製するノード数は $O(\log N)$ である。

## 注意点

- version 0 が初期状態で、`0 <= version < versions()` の version だけを参照できる。
- 更新と `fork` は指定した version を変更せず、新しい version 番号を返す。`latest_version()` は `versions() - 1` である。
- 点は実際の範囲内、区間は半開区間 $[l, r)$ で指定する。
- 境界探索の述語は単調で、単位元に対して `true` を返す必要がある。
- 不正な version・点・区間、述語が単位元で `false`、version・ノードの容量超過では `std::runtime_error` を送出する。
- 容量不足で更新が失敗した場合、version 数と使用ノード数は更新前に戻る。
- コピーコンストラクタとコピー代入は削除されている。
