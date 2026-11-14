---
title: Partially Persistent Segment Tree Beats (部分永続セグメント木Beats)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_segtree_beats.hpp
---

部分永続 Segment Tree Beats。更新は最新 version に対してだけ行い、木の各添字の変更履歴から過去の version を読む。完全永続版には依存しない。

```cpp
PartiallyPersistentSegmentTreeBeats<T, MAX_SIZE, MAX_VERSIONS, MAX_CHANGES>
```

- `MAX_SIZE`: 列の長さの上限
- `MAX_VERSIONS`: version 0 を除く更新回数の上限
- `MAX_CHANGES`: 全ノードの変更履歴の合計の上限

容量（3 つのテンプレート引数）をすべて省略すると可変長版になり、`PartiallyPersistentSegmentTreeBeats<T> tree(n);` のように長さを実行時に与える。履歴は使用量に応じて `std::vector` を拡張する。

## コンストラクタ

```cpp
explicit PartiallyPersistentSegmentTreeBeats(int n = MAX_SIZE)  // 可変長版の既定値は 0
explicit PartiallyPersistentSegmentTreeBeats(const std::vector<T>& values)
explicit PartiallyPersistentSegmentTreeBeats(const std::array<T, N>& values)
```

長さ `n` の全 `T{}` の列、または `values` を version 0 として作る。

## 更新

```cpp
int range_chmin(int l, int r, const T& x)
int range_chmax(int l, int r, const T& x)
int range_add(int l, int r, const T& x)
int set(int k, const T& x)
```

最新 version を更新した新しい version を作り、その番号を返す。容量不足で失敗した場合は、変更履歴と version 数を更新前に戻す。

## クエリ

```cpp
T get(int version, int k) const
T range_sum(int version, int l, int r) const
T range_min(int version, int l, int r) const
T range_max(int version, int l, int r) const
T all_sum(int version) const
T all_min(int version) const
T all_max(int version) const
```

`version` を省略すると最新 version を読む。過去の version に対するクエリでは、祖先ノードの加算・上限制約・下限制約を子へ順に合成して値を求める。

```cpp
int size() const
int versions() const
int latest_version() const
int changes_used() const
```

順に列の長さ、version 数、最新 version 番号、変更履歴数を返す。

## 時間計算量

$N$ を列の長さ、$C = \texttt{MAX_CHANGES}$ とする。

- コンストラクタ: $O(\texttt{MAX_SIZE}+C)$
- `range_chmin`, `range_chmax`: 償却 $O(\log^2 N)$
- `range_add`, `set`: $O(\log N)$
- 最新 version の `get`, `range_sum`, `range_min`, `range_max`: $O(\log N)$
- 過去 version の同じクエリ: $O(\log N\log C)$
- 最新 version の `all_*`: $O(1)$、過去 version の `all_*`: $O(\log C)$
- version・履歴情報: $O(1)$

空間計算量は $O(\texttt{MAX_SIZE}+C)$ である。

## 注意点

- version 0 が初期状態で、クエリは `0 <= version < versions()` の任意の version を参照できる。
- 更新は常に `latest_version()` を元に行い、新しい version 番号を返す。過去の version を更新元にする API はない。
- 点は列内、区間は半開区間 $[l, r)$ である。
- 空区間に対する `range_min`, `range_max`、空列に対する `all_min`, `all_max` は `std::runtime_error` を送出する。
- 不正な version・点・区間、version・履歴の容量超過では `std::runtime_error` を送出する。
- 可変長版で $n \gt 2^{28}$ の場合は `std::length_error` になる。
