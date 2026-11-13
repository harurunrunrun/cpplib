---
title: Dynamic Monochromatic Tree (動的同色連結成分木)
documentation_of: ../../../../src/structure/tree/query/dynamic_monochromatic_tree.hpp
---

2 色で塗られた形の変わらない木について、頂点色の変更、頂点値の変更、指定した頂点を含む同色連結成分の
モノイド積の取得を処理する。同色連結成分とは、両端を含むパス上の全頂点が同じ色である頂点集合をいう。

## テンプレート引数

```cpp
DynamicMonochromaticTree<Monoid, MAX_SIZE>
```

`Monoid` は次を持つモノイドのオブジェクト（非型テンプレート引数）、`MAX_SIZE` は頂点数の上限である。`MAX_SIZE` は省略でき、デフォルト値 `-1` では可変長版になる。

```cpp
using S = typename decltype(Monoid)::S;
S Monoid.op(S left, S right);
S Monoid.e();
```

連結成分は頂点集合なので、`Monoid.op` は結合的かつ可換でなければならない。
`Monoid.e()` は単位元を返す。

## 構築

```cpp
DynamicMonochromaticTree(int n = MAX_SIZE)
DynamicMonochromaticTree(
    const std::vector<S>& values,
    const std::vector<bool>& colors = {}
)
void add_edge(int u, int v)
void build(int root = 0)
```

頂点は 0-indexed である。頂点数だけを渡した場合、初期値はすべて `Monoid.e()`、初期色はすべて `false` である。
`values` を渡す場合、空の `colors` は全頂点が `false` であることを表す。空でない `colors` の長さは
`values` と一致しなければならない。

`add_edge` で無向辺をちょうど `n - 1` 本追加してから `build` を呼ぶ。
`root` は内部の根付き木の根であり、答えには影響しない。`build` は現在の色と値を使って
再実行できる。オブジェクトはコピーできず、ムーブできる。

## 頂点数と根

```cpp
int size() const
int root() const
```

`size` は頂点数を返す。`root` は最後の `build` で指定した根を返す。
空の木を `build()` した後の `root()` は `-1` である。

## 色

```cpp
bool color(int v) const
bool set_color(int v, bool new_color)
bool toggle(int v)
```

`color` は現在の色を返す。`set_color` は色が変化したとき `true` を、既に同じ色なら `false` を返す。
`toggle` は色を反転し、反転後の色を返す。これらは `build` 前にも呼べる。

## 頂点値

```cpp
void set(int v, const S& value)
S get(int v) const
```

`set` は頂点値を変更し、`get` は現在の値を返す。どちらも `build` 前にも呼べる。

## 同色連結成分の積

```cpp
S component_prod(int v)
```

`v` と同じ同色連結成分に属する全頂点の値を、各頂点ちょうど 1 回ずつ `Monoid.op` で
まとめた値を返す。

## アルゴリズム

木を `root` で根付ける。色 $c$ ごとに、根から頂点 $x$ までのパスにある
色 $c$ 以外の頂点数を $h_c(x)$ とする。頂点 $u$ の色の変更は、$u$ の部分木全体に対する
$h_0$, $h_1$ の区間加算になる。

頂点 $v$ と異なる色を持つ最も深い祖先を $b$、$b$ の直下で $v$ 側にある頂点を $a$ とする。
該当する $b$ がなければ $a$ を根とする。このとき $v$ の同色連結成分は、$a$ の部分木で
$h_{\mathrm{color}(v)}$ が最小となる頂点全体と一致する。

HLD で $b$ を求め、区間最小値と最小値を取る頂点のモノイド積を持つ遅延セグメント木で
答えを求める。色ごとにセグメント木を 1 本ずつ持つため、星形の木の中心の色を変更しても隣接辺を
列挙しない。

## 時間計算量

$N$ を頂点数とし、`Monoid.op` とコピーを $O(1)$ とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は $O(N)$
- `add_edge`: $O(1)$
- `build`: $O(N)$
- `size`, `root`, `color`, `get`: $O(1)$
- `set`: `build` 前は $O(1)$、`build` 後は $O(\log N)$
- `set_color`, `toggle`: `build` 前は $O(1)$、`build` 後は $O(\log N)$
- `component_prod`: $O(\log^2 N)$

## 空間計算量

オブジェクトの保存領域は固定長版で $O(\texttt{MAX_SIZE})$、可変長版で $O(N)$ である。

| API | 追加空間計算量 |
| --- | --- |
| コンストラクタ、ムーブ、`size`, `root`, `add_edge`, `color`, `get` | $O(1)$ |
| `build` | セグメント木構築の再帰スタック $O(\log N)$ |
| `set` | `build` 前は $O(1)$、後は再帰スタック $O(\log N)$ |
| `set_color`, `toggle` | `build` 前は $O(1)$、後は再帰スタック $O(\log N)$ |
| `component_prod` | 再帰スタック $O(\log N)$ |
| コピー構築、コピー代入 | 使用不可 |

## 注意点

- `component_prod` と `root` は `build` 後に呼ぶ。
- `build` 後に辺は追加できない。
- 頂点、頂点数、根、辺の容量が範囲外なら `runtime_error` を送出する。
- 辺数が $N-1$ でない、閉路がある、または非連結なら `build` は `runtime_error` を送出する。
- `S` はデフォルト構築とコピーが可能でなければならない。
- `Monoid.op` の結果に対するオーバーフロー検査は行わない。
- 容量を指定した固定配列版（`DynamicMonochromaticTree<Monoid, N>`）と可変長版（`DynamicMonochromaticTree<Monoid>`）は同じヘッダで利用できる。
