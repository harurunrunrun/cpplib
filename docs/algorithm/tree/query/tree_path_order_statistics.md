---
title: Tree Path Order Statistics (木上パス順序統計)
documentation_of: ../../../../src/algorithm/tree/query/tree_path_order_statistics.hpp
---

頂点に値を持つ静的な木に対し、パス上の $k$ 番目の値や、値域内にある値の個数を求める。
根から各頂点までの値の頻度を永続セグメント木で保持し、加算する 2 本の根と減算する 2 本の根を同時に降りる。

## 使い方

```cpp
TreePathOrderStatistics<long long> statistics(n);              // 可変長版
// TreePathOrderStatistics<long long, 200000> statistics(n);   // 固定容量版
statistics.add_edge(u, v);
statistics.build(values);
auto value = statistics.kth_smallest(u, v, k);
```

## API

以下、$C$ を容量（固定容量版では $\texttt{MAX_SIZE}$、可変長版では $n$）とする。

### `TreePathOrderStatistics<Value, MAX_SIZE>(n)`

```cpp
template<class Value, int MAX_SIZE = -1>
struct TreePathOrderStatistics;
explicit TreePathOrderStatistics(int n = (MAX_SIZE < 0 ? 0 : MAX_SIZE));
```

頂点数 `n` の空の構造を作る。頂点番号は $[0, n)$ とする。
`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると、実行時の頂点数の上限となる。

- 時間計算量: $O(C\log(C+1))$
- 空間計算量: $O(C\log(C+1))$

### コピー・ムーブ

コピー構築とコピー代入は使用できない。ムーブ構築とムーブ代入は、構築済みの状態を移す。

- ムーブ構築の時間計算量: $O(1)$
- ムーブ代入の時間計算量: 代入先が保持していた `Value` の破棄を含めて最悪 $O(n)$
- 追加空間計算量: $O(1)$

### `size()`

頂点数を返す。

- 時間計算量: $O(1)$

### `add_edge(u, v)`

無向辺を追加する。`build` より前に、ちょうど $n-1$ 本追加する。

- 時間計算量: $O(1)$

### `build(values, root = 0)`

`values[v]` を頂点 `v` の値として、`root` を根とする索引を構築する。
同じ辺集合に対して、別の値で再構築できる。

- 時間計算量: $O(C+n\log(C+1)+n\log(n+1))$
- 追加空間計算量: $O(n)$

### `root()`

最後の `build` で指定した根を返す。空木では `-1` を返す。

- 時間計算量: $O(1)$

### `parent(v)` / `depth(v)`

根付き木における頂点 `v` の親と深さを返す。根の親は `-1` である。

- 時間計算量: $O(1)$

### `value(v)`

頂点 `v` の座標圧縮後の値を返す。元の `values[v]` と `operator<` の意味で同値な値だが、同値な別のオブジェクトが入力されていた場合は、その同値類の代表値になる。

- 時間計算量: $O(1)$

### `lca(u, v)`

頂点 `u`, `v` の最小共通祖先を返す。

- 時間計算量: $O(\log(C+1))$

### `path_size(u, v)`

`u` から `v` までのパスに含まれる頂点数を返す。

- 時間計算量: $O(\log(C+1))$

### `kth_smallest(u, v, k)`

パス上の値を昇順に並べたときの、0-indexed で `k` 番目の値を返す。重複する値はそれぞれ数える。

- 時間計算量: $O(\log(C+1))$

### `kth_largest(u, v, k)`

パス上の値を降順に並べたときの、0-indexed で `k` 番目の値を返す。

- 時間計算量: $O(\log(C+1))$

### `count_less(u, v, upper)`

パス上で `upper` 未満の値の個数を返す。

- 時間計算量: $O(\log(C+1))$

### `count_equal(u, v, target)`

パス上で `target` と等しい値の個数を返す。

- 時間計算量: $O(\log(C+1))$

### `range_freq(u, v, lower, upper)`

パス上で半開区間 `[lower, upper)` に属する値の個数を返す。

- 時間計算量: $O(\log(C+1))$

## API 別の空間計算量

| API | 空間計算量 |
| --- | --- |
| `size`, `add_edge`, `root`, `parent`, `depth`, `value` | 追加 $O(1)$ |
| `lca`, `path_size`, `kth_smallest`, `kth_largest` | 追加 $O(1)$ |
| `count_less`, `count_equal`, `range_freq` | 追加 $O(1)$ |

## 注意点

- `Value` はコピー可能で、`operator<` が strict weak ordering を満たす必要がある。値の等価性は `!(a < b) && !(b < a)` で判定する。
- `n` が負（固定容量版では `MAX_SIZE` を超える場合も）、頂点または `k` が範囲外、`range_freq` で `upper < lower`、`values.size() != n`、入力が木でない、`build` 前に索引 API を呼んだ場合は `std::runtime_error` を送出する。ノード番号が `int` に収まらない場合は `std::length_error` を送出する。
- `build` の後に辺は追加できない。
