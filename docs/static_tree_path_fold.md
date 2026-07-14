---
title: Static Tree Path Fold (静的木上パス積) [DISQUERY] [QTREE2]
documentation_of: ../src/algorithm/tree/static_tree_path_fold.hpp
---

辺にモノイド要素を持つ静的な木を二分 lifting で前処理し、任意の有向パス上の積を求める。
各 doubling 区間を上向き・下向きの両順序で保持するため、モノイドは可換でなくてもよい。

```cpp
using Range = std::pair<int, int>;
constexpr Range op(Range a, Range b){
    return {std::min(a.first, b.first), std::max(a.second, b.second)};
}
constexpr Range e(){ return {INT_MAX, INT_MIN}; }
constexpr Monoid<op, e> range_monoid{};

StaticTreePathFold<range_monoid, 100000> tree(n);
tree.add_edge(u, v, {weight, weight});
tree.build();
const auto [minimum, maximum] = tree.prod(u, v);
```

以下ではモノイド演算・単位元生成・`S` のcopyを $O(1)$ と数える。

## API

### `StaticTreePathFold<Monoid, MAX_SIZE>(n = MAX_SIZE)`

頂点数 `n` の空の構造を作る。`Monoid` は `S`, `op(S,S)`, `e()` を持つ
`Monoid` object、`MAX_SIZE` は正の頂点数上限である。

- 時間計算量: $O(n\log(n+1))$
- 空間計算量: $O(n\log(n+1))$

### `using S`

辺値とパス積の型であり、`Monoid::S` と同じ型である。型の参照は $O(1)$。

### copy / move

copy構築・copy代入は、辺集合と構築済み索引を複製する。move構築・move代入は保持領域を移す。

- copy: $O(n\log(n+1))$ 時間・領域
- move構築: $O(1)$ 時間・追加領域
- move代入: 代入先の保持領域の破棄を含めて最悪 $O(n\log(n+1))$ 時間、追加 $O(1)$ 領域

### `size()`

頂点数を返す。

- 時間計算量・追加空間計算量: $O(1)$

### `add_edge(left, right, value)`

無向辺 `left-right` を追加し、どちら向きに通る場合も辺要素を `value` とする。
`build` より前にちょうど `n-1` 本追加する。

- 時間計算量: 償却 $O(1)$
- 追加空間計算量: 償却 $O(1)$

### `build(root = 0)`

`root` を根として、親・深さ・祖先と両向きのパス積tableを構築する。
辺を変更せず別の根で再構築できる。

- 時間計算量: $O(n\log(n+1))$
- 追加空間計算量: $O(n)$

### `root()`

最後の `build` で指定した根を返す。空木では `-1` を返す。

- 時間計算量・追加空間計算量: $O(1)$

### `parent(vertex)` / `depth(vertex)`

根付き木での親と深さを返す。根の親は `-1`、根の深さは0である。

- 時間計算量・追加空間計算量: $O(1)$

### `jump(vertex, distance)`

`vertex` から親方向へちょうど `distance` 辺進んだ頂点を返す。

- 時間計算量: $O(\log(n+1))$
- 追加空間計算量: $O(1)$

### `lca(left, right)`

2頂点の最小共通祖先を返す。

- 時間計算量: $O(\log(n+1))$
- 追加空間計算量: $O(1)$

### `dist(left, right)`

2頂点間の辺数を返す。

- 時間計算量: $O(\log(n+1))$
- 追加空間計算量: $O(1)$

### `kth_vertex_on_path(left, right, index_on_path)`

`left` から `right` への頂点列で、0-indexedの `index_on_path` 番目の
頂点を返す。両端も頂点列に含む。

- 時間計算量: $O(\log(n+1))$
- 追加空間計算量: $O(1)$

### `prod(left, right)`

`left` から `right` へ進む順に、パス上の辺要素を `Monoid.op` で畳み込む。
同じ頂点を指定した空パスでは `Monoid.e()` を返す。

- 時間計算量: $O(\log(n+1))$
- 追加空間計算量: $O(1)$

## 注意点

- `Monoid.op` は結合則を、`Monoid.e()` は左右の単位元を満たす必要がある。
- 辺値は方向によらず同じ要素を使うが、`prod(u,v)` と `prod(v,u)` は積の順序が逆になる。
- `n` は `[0,MAX_SIZE]`、頂点は `[0,n)` に入る必要がある。
- `build` 前の索引API、範囲外の頂点・根・jump距離、`build` 後の辺追加、連結木でない入力では `runtime_error` を送出する。
- `build` 後は辺を変更できない。別の根での `build` は可能であり、パス積自体は根の選び方に依存しない。
- 再度 `build` を呼ぶと以前の索引は直ちに無効になる。再構築が例外で失敗した場合、索引APIは次の正常な `build` まで `runtime_error` を送出する。
