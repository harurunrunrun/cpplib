---
title: Persistent Dynamic Segment Tree (完全永続動的セグメント木)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_dynamic_segtree.hpp
---

巨大な添字範囲を扱う完全永続セグメント木。更新ごとに根から更新点までの経路を複製する。

```cpp
PersistentDynamicSegtree<Monoid, MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

- `MAX_SIZE`: 添字範囲 $[0, \texttt{MAX_SIZE})$ の長さ
- `MAX_NODES`: 全 version で共有するノード数の上限
- `MAX_VERSIONS`: version 0 を除く version 数の上限

固定容量版はデフォルトコンストラクタで構築する。容量（3 つのテンプレート引数）をすべて省略すると可変長版になり、`PersistentDynamicSegtree<Monoid> tree(n);` のように長さを実行時に与える。ノードと version は使用量に応じて `std::vector` を拡張する。

## 更新

```cpp
int set(int version, long long index, S value)
int fork(int version)
```

新しい version を作り、その番号を返す。`set` は指定 version の 1 点を書き換えて分岐し、`fork` は内容を変えずに複製する。初期 version は 0 である。

## 取得

```cpp
S get(int version, long long index) const
S prod(int version, long long l, long long r) const
S all_prod(int version) const
long long max_right(int version, long long l, F f, Args... args) const
long long min_left(int version, long long r, F f, Args... args) const
int versions() const
int latest_version() const
int nodes_used() const
```

述語は追加引数 `args...` を受け取れる。

## 時間計算量

- コンストラクタ: 固定容量版は $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS})$
- `set`, `get`, `prod`, `max_right`, `min_left`: $O(\log \texttt{MAX_SIZE})$
- `fork`, `all_prod`, `versions`, `latest_version`, `nodes_used`: $O(1)$

`set` で複製するノード数は $O(\log \texttt{MAX_SIZE})$ である。

## 注意点

- version 0 が初期状態で、`0 <= version < versions()` の version だけを参照できる。
- 更新と `fork` は指定した version を変更せず、新しい version 番号を返す。`latest_version()` は `versions() - 1` である。
- 点は実際の範囲内、区間は半開区間 $[l, r)$ で指定する。
- 境界探索の述語は単調で、単位元に対して `true` を返す必要がある。
- 不正な version・点・区間、述語が単位元で `false`、version・ノードの容量超過では `std::runtime_error` を送出する。
- 容量不足で更新が失敗した場合、version 数と使用ノード数は更新前に戻る。
- コピーコンストラクタとコピー代入は削除されている。
