---
title: Persistent Wide Segtree (完全永続ワイドセグメント木)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_widesegtree.hpp
---

`long long` の和を扱う完全永続な疎セグメント木。`PersistentDynamicSegtree` に和のモノイドを与えたラッパーである。

```cpp
PersistentWideSegtree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

別名 `PersistentWide_Segtree` も定義されている。固定容量版はデフォルトコンストラクタで構築し、範囲は $[0, \texttt{MAX_SIZE})$ になる。容量（3 つのテンプレート引数）をすべて省略すると可変長版になり、`PersistentWideSegtree<> tree(n);` のように長さを実行時に与える。

## API

```cpp
int set(int version, long long index, long long value)
int add(int version, long long index, long long value)
int fork(int version)
long long get(int version, long long index) const
long long prod(int version, long long l, long long r) const
long long all_prod(int version) const
int versions() const
int latest_version() const
int nodes_used() const
```

- `set`, `add`: 指定 version の 1 点に代入・加算した新しい version を作り、その番号を返す。
- `fork`: 指定 version と同じ内容の新しい version を作る。
- `get`, `prod`, `all_prod`: 指定 version の値・半開区間 $[l, r)$ の和・全体の和を返す。
- `fork` 以外の更新・取得関数は `version` を省略でき、そのときは最新 version を使う。

## 時間計算量

- コンストラクタ: $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS})$
- `set`, `add`, `get`, `prod`: $O(\log \texttt{MAX_SIZE})$
- `fork`, `all_prod`, `versions`, `latest_version`, `nodes_used`: $O(1)$

## 注意点

- version 0 が初期状態（全要素 0）で、`0 <= version < versions()` の version だけを参照できる。
- 更新と `fork` は指定した version を変更せず、新しい version 番号を返す。`latest_version()` は `versions() - 1` である。
- 点は実際の範囲内、区間は半開区間 $[l, r)$ で指定する。
- 不正な version・点・区間、version・ノードの容量超過では `std::runtime_error` を送出する。
- 容量不足で更新が失敗した場合、version 数と使用ノード数は更新前に戻る。
