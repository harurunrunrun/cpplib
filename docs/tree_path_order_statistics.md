---
title: Tree Path Order Statistics (木上パス順序統計)
documentation_of: ../src/algorithm/tree/query/tree_path_order_statistics.hpp
---

静的な頂点値付き木に対し、パス上のk番目の値や値域内の個数を求める。
根から各頂点までの頻度を永続セグメント木で保持し、2本の加算rootと2本の減算rootを同時に降りる。

```cpp
TreePathOrderStatistics<long long, 200000> statistics(n);
statistics.add_edge(u, v);
statistics.build(values);
auto value = statistics.kth_smallest(u, v, k);
```

## API

### `TreePathOrderStatistics<Value, MAX_SIZE>(n = MAX_SIZE)`

頂点数 `n` の空の構造を作る。頂点番号は $[0,n)$ とする。
`MAX_SIZE` は正で、実行時の頂点数の上限を表す。

- 時間計算量: $O(\mathrm{MAX\_SIZE}\log(\mathrm{MAX\_SIZE}+1))$
- 空間計算量: $O(\mathrm{MAX\_SIZE}\log(\mathrm{MAX\_SIZE}+1))$

### copy / move

copy構築とcopy代入は使用できない。move構築とmove代入は、構築済み状態を移す。

- move構築の時間計算量: $O(1)$
- move代入の時間計算量: 代入先が保持していた `Value` の破棄を含めて最悪 $O(n)$
- 追加空間計算量: $O(1)$

### `size()`

頂点数を返す。

- 時間計算量: $O(1)$

### `add_edge(u, v)`

無向辺を追加する。`build` より前にちょうど $n-1$ 本追加する。

- 時間計算量: $O(1)$

### `build(values, root = 0)`

`values[v]` を頂点 `v` の値として、`root` を根に索引を構築する。
同じ辺集合と別の値で再構築できる。

- 時間計算量: $O(\mathrm{MAX\_SIZE}+n\log(\mathrm{MAX\_SIZE}+1)+n\log(n+1))$
- 追加空間計算量: $O(n)$

### `root()`

最後の `build` で指定した根を返す。空木では `-1`。

- 時間計算量: $O(1)$

### `parent(v)` / `depth(v)`

根付き木における頂点 `v` の親と深さを返す。根の親は `-1`。

- 時間計算量: $O(1)$

### `value(v)`

頂点 `v` の座標圧縮値を返す。元の `values[v]` と `operator<` 上で同値な値だが、
同値な別objectが入力されていた場合はその同値類の代表値になる。

- 時間計算量: $O(1)$

### `lca(u, v)`

頂点 `u`, `v` の最小共通祖先を返す。

- 時間計算量: $O(\log(\mathrm{MAX\_SIZE}+1))$

### `path_size(u, v)`

`u` から `v` までのパスに含まれる頂点数を返す。

- 時間計算量: $O(\log(\mathrm{MAX\_SIZE}+1))$

### `kth_smallest(u, v, k)`

パス上の値を昇順に並べたときの0-indexed `k` 番目を返す。重複する値はそれぞれ数える。

- 時間計算量: $O(\log(\mathrm{MAX\_SIZE}+1))$

### `kth_largest(u, v, k)`

パス上の値を降順に並べたときの0-indexed `k` 番目を返す。

- 時間計算量: $O(\log(\mathrm{MAX\_SIZE}+1))$

### `count_less(u, v, upper)`

パス上で `upper` 未満の値の個数を返す。

- 時間計算量: $O(\log(\mathrm{MAX\_SIZE}+1))$

### `count_equal(u, v, target)`

パス上で `target` と等しい値の個数を返す。

- 時間計算量: $O(\log(\mathrm{MAX\_SIZE}+1))$

### `range_freq(u, v, lower, upper)`

パス上で半開区間 $[\mathtt{lower},\mathtt{upper})$ に属する値の個数を返す。

- 時間計算量: $O(\log(\mathrm{MAX\_SIZE}+1))$

## API別の空間計算量

| API | 空間計算量 |
| --- | --- |
| `size`, `add_edge`, `root`, `parent`, `depth`, `value` | 追加 $O(1)$ |
| `lca`, `path_size`, `kth_smallest`, `kth_largest` | 追加 $O(1)$ |
| `count_less`, `count_equal`, `range_freq` | 追加 $O(1)$ |

## 注意点

`Value` はcopy可能で、`operator<` がstrict weak orderingを満たす必要がある。
値の等価性は `!(a < b) && !(b < a)` で判定する。

`n` が $[0,\mathrm{MAX\_SIZE}]$ の外、頂点または `k` が範囲外、
`range_freq` の `upper < lower`、`values.size() != n`、入力が木でない、
`build` 前に索引APIを呼んだ場合は `runtime_error` を送出する。
`build` 後に辺は追加できない。
