---
title: Dynamic Monochromatic Tree (動的同色連結成分木)
documentation_of: ../src/structure/tree/dynamic_monochromatic_tree.hpp
---

2色で塗られた静的な木について、頂点色の変更、頂点値の変更、指定頂点を含む同色連結成分の
Monoid積を処理する。同色連結成分とは、両端を含むパス上の全頂点が同じ色である頂点集合をいう。

# テンプレート引数

```cpp
DynamicMonochromaticTree<Monoid, MAX_SIZE>
```

`Monoid` は次を持つMonoid object、`MAX_SIZE` は頂点数の上限。

```cpp
using S = typename decltype(Monoid)::S;
S Monoid.op(S left, S right);
S Monoid.e();
```

連結成分は頂点集合なので、`Monoid.op` は結合的かつ可換でなければならない。
`Monoid.e()` は単位元を返す。

# 構築

```cpp
DynamicMonochromaticTree(int n = MAX_SIZE)
DynamicMonochromaticTree(
    const std::vector<S>& values,
    const std::vector<bool>& colors = {}
)
void add_edge(int u, int v)
void build(int root = 0)
```

頂点は0-indexed。頂点数だけを渡した場合、初期値は全て `Monoid.e()`、初期色は全てfalse。
`values` を渡す場合、空の `colors` は全頂点falseを表す。空でない `colors` の長さは
`values` と一致しなければならない。

`add_edge` で無向辺を追加し、ちょうど `n-1` 辺を追加してから `build` を呼ぶ。
`root` は内部の根付き木の根であり、答えには影響しない。`build` は現在の色と値を使って
再実行できる。オブジェクトはcopyできず、moveできる。

# 頂点数と根

```cpp
int size() const
int root() const
```

`size` は頂点数を返す。`root` は最後の `build` で指定した根を返す。
空の木を `build()` した後の `root()` は `-1`。

# 色

```cpp
bool color(int v) const
bool set_color(int v, bool new_color)
bool toggle(int v)
```

`color` は現在の色を返す。`set_color` は色が変化したときtrue、既に同じ色ならfalseを返す。
`toggle` は色を反転し、反転後の色を返す。これらは `build` 前にも呼べる。

# 頂点値

```cpp
void set(int v, const S& value)
S get(int v) const
```

`set` は頂点値を変更し、`get` は現在の値を返す。どちらも `build` 前にも呼べる。

# 同色連結成分積

```cpp
S component_prod(int v)
```

`v` と同じ同色連結成分に属する全頂点の値を、各頂点ちょうど1回ずつ `Monoid.op` で
まとめた値を返す。

# アルゴリズム

木を `root` で根付ける。色 `c` ごとに、根から頂点 `x` までのパスにある
`c` 以外の頂点数を `h_c(x)` とする。頂点 `u` の色変更は、`u` の部分木全体に対する
`h_0`, `h_1` の区間加算になる。

頂点 `v` と異なる色の最深祖先を `b`、`b` の直下で `v` 側の頂点を `a` とする。
該当する `b` がなければ `a=root` とする。このとき `v` の同色連結成分は、`a` の部分木で
`h_color(v)` が最小となる頂点全体と一致する。

HLDで `b` を求め、区間最小値と最小値を取る頂点のMonoid積を持つlazy segment treeで
答えを求める。色ごとにsegment treeを1本ずつ持つため、星形の中心を変更しても隣接辺を
列挙しない。

# 時間計算量

`Monoid.op` とcopyを $O(1)$ とする。

- コンストラクタ: $O(\mathtt{MAX\_SIZE})$
- `add_edge`: $O(1)$
- `build`: $O(N)$
- `size`, `root`, `color`, `get`: $O(1)$
- `set`: `build` 前は $O(1)$、`build` 後は $O(\log N)$
- `set_color`, `toggle`: `build` 前は $O(1)$、`build` 後は $O(\log N)$
- `component_prod`: $O(\log^2 N)$

# 空間計算量

objectの保存領域は $O(\mathtt{MAX\_SIZE})$。

| API | 追加空間計算量 |
| --- | --- |
| コンストラクタ、move、`size`, `root`, `add_edge`, `color`, `get` | $O(1)$ |
| `build` | segment tree構築のcall stack $O(\log N)$ |
| `set` | `build` 前は $O(1)$、後はcall stack $O(\log N)$ |
| `set_color`, `toggle` | `build` 前は $O(1)$、後はcall stack $O(\log N)$ |
| `component_prod` | call stack $O(\log N)$ |
| copy構築、copy代入 | 使用不可 |

# 注意点

- `component_prod` と `root` は `build` 後に呼ぶ。
- `build` 後に辺は追加できない。
- 頂点、頂点数、根、辺容量が範囲外なら `runtime_error`。
- 辺数が `N-1` でない、閉路がある、または非連結なら `build` は `runtime_error`。
- `S` はdefault constructionとcopyが可能でなければならない。
- `Monoid.op` の結果に対するoverflow検査は行わない。
