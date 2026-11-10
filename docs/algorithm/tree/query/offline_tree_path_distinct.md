---
title: Offline Tree Path Distinct (木上パス異なる値数) [COT2]
documentation_of: ../../../../src/algorithm/tree/query/offline_tree_path_distinct.hpp
---

頂点に値を持つ静的な木について、複数のパスに含まれる相異なる値の個数をオフラインで返す。
内部では `TreeMo`（木上の Mo のアルゴリズム）を使う。

## API

### `OfflineTreePathDistinct<Value, MAX_SIZE>(vertex_values)`

```cpp
template<class Value, int MAX_SIZE = -1>
struct OfflineTreePathDistinct;
explicit OfflineTreePathDistinct(const std::vector<Value>& vertex_values);
```

各頂点の値を保持する。時間計算量は $O(n)$（固定容量版では `TreeMo` の表の初期化を含む）である。
`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると固定容量の配列を使う。

```cpp
OfflineTreePathDistinct<int> dynamic(values);
OfflineTreePathDistinct<int, 100000> fixed(values);
```

### `add_edge(left, right)`

無向辺を追加する。時間計算量は償却 $O(1)$ である。

### `build(root = 0)`

Euler tour と値の座標圧縮を構築する。時間計算量は $O(n\log n)$ である。

### `add_query(left, right)`

パスのクエリを追加し、その番号を返す。時間計算量は償却 $O(\log n)$ である。

### `solve()`

各クエリの答え（相異なる値の個数）を追加順に返す。
クエリ数を $Q$ とすると、時間計算量は $O(Q\log Q + n\sqrt{Q} + n + Q)$ である。

## 注意点

- 辺集合は連結な木でなければならない。固定容量版では頂点数が `MAX_SIZE` 以下でなければならない。
- `build` の後にクエリを追加し、すべて追加してから `solve` を呼ぶ。
- 範囲外の頂点や不正な木では `std::runtime_error` を送出する。
