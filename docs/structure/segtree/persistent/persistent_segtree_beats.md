---
title: Persistent Segment Tree Beats (完全永続セグメント木Beats)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_segtree_beats.hpp
---

完全永続 Segment Tree Beats。更新で変更するノードだけを経路複製（path copy）し、任意の version から分岐できる。

```cpp
PersistentSegmentTreeBeats<T, MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

- `MAX_SIZE`: 列の長さの上限
- `MAX_NODES`: 初期の木と全更新で作るノード数の合計の上限
- `MAX_VERSIONS`: version 0 を除いて追加できる version 数の上限

容量（3 つのテンプレート引数）をすべて省略すると可変長版になり、`PersistentSegmentTreeBeats<T> tree(n);` のように長さを実行時に与える。ノードと version は使用量に応じて `std::vector` を拡張する。

## コンストラクタ

```cpp
explicit PersistentSegmentTreeBeats(int n = MAX_SIZE)  // 可変長版の既定値は 0
explicit PersistentSegmentTreeBeats(const std::vector<T>& values)
explicit PersistentSegmentTreeBeats(const std::array<T, N>& values)
```

長さ `n` の全 `T{}` の列、または `values` を version 0 として作る。

## 更新

```cpp
int range_chmin(int version, int l, int r, const T& x)
int range_chmax(int version, int l, int r, const T& x)
int range_add(int version, int l, int r, const T& x)
int set(int version, int k, const T& x)
int fork(int version)
```

指定 version から新しい version を作り、その番号を返す。容量不足時は例外を送出し、ノード数と version 数を更新前に戻す。

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

クエリは木を書き換えない。祖先ノードに残る加算・上限制約・下限制約を子へ順に合成して評価する。

```cpp
int size() const
int versions() const
int latest_version() const
int nodes_used() const
```

順に列の長さ、version 数、最新 version 番号、使用ノード数を返す。

## 時間計算量

$N$ を列の長さとする。

- コンストラクタ: $O(\texttt{MAX_SIZE}+\texttt{MAX_NODES}+\texttt{MAX_VERSIONS})$
- `range_chmin`, `range_chmax`: 償却 $O(\log^2 N)$
- `range_add`, `set`, `get`, `range_sum`, `range_min`, `range_max`: $O(\log N)$
- `fork`, `all_sum`, `all_min`, `all_max`, version・ノード情報: $O(1)$

空間計算量は $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS})$ である。

## 注意点

- version 0 が初期状態で、`0 <= version < versions()` の version だけを参照できる。
- 更新と `fork` は指定した version を変更せず、新しい version 番号を返す。`latest_version()` は `versions() - 1` である。
- 点は実際の列内、区間は 0-indexed の半開区間 $[l, r)$ で指定する。
- 空区間に対する `range_min`, `range_max`、空列に対する `all_min`, `all_max` は `std::runtime_error` を送出する。
- 不正な version・点・区間、version・ノードの容量超過では `std::runtime_error` を送出する。
- 容量不足で更新が失敗した場合、version 数と使用ノード数は更新前に戻る。
