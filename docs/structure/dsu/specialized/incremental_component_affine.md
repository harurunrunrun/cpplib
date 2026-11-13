---
title: Incremental Component Affine (増分連結成分アフィン作用)
documentation_of: ../../../../src/structure/dsu/specialized/incremental_component_affine.hpp
---

頂点値を持つ増分無向グラフについて、辺の追加、連結成分全体へのアフィン作用、頂点値と成分和の取得を処理する。
成分の併合履歴を木（Link-Cut Tree）で持ち、作用を頂点から現在の履歴の根まで順序付きで合成する。
したがって、併合前に適用した作用が、後から連結された頂点に適用されることはない。

## テンプレート引数

```cpp
IncrementalComponentAffine<MonoidActLen, MAX_SIZE = -1>
```

`MonoidActLen` は `structure/types/monoid/monoid_act_len.hpp` と同じく、次の要素を持つオブジェクト（非型テンプレート引数）である。

- 集約値の型 `S` と作用の型 `T`
- 可換な成分集約 `op(first, first_length, second, second_length)` と単位元 `e()`
- `mapping(action, aggregate, length)`
- `composition(newer, older)` と恒等作用 `id()`

作用 `older` の後に `newer` を適用する合成が `composition(newer, older)` である。
`S` と `T` はデフォルト構築・コピー・代入ができる必要がある。

`MAX_SIZE` は頂点数の上限（$10^9$ 以下）。省略（`-1`）すると、実行時の頂点数に合わせて `std::vector` を確保する可変長版になる。
コピーはできず、ムーブはできる。

## コンストラクタ

```cpp
explicit IncrementalComponentAffine(int n = MAX_SIZE);
explicit IncrementalComponentAffine(const vector<S>& values);
```

$n$ 個の単独の頂点を値 `e()` で作る。または `values[v]` を頂点 $v$ の初期値として作る。可変長版では `n` のデフォルトは $0$ である。

## add_edge

```cpp
bool add_edge(int first, int second);
```

無向辺を追加する。異なる成分を併合した場合だけ `true` を返す。
自己ループと同じ成分内の辺は状態を変えず、`false` を返す。

## apply_component

```cpp
void apply_component(int vertex, const T& action);
```

`vertex` を含む現在の連結成分に属するすべての頂点に `action` を適用する。
後から辺で加わる頂点には、この作用を遡って適用しない。

## vertex_value / component_sum

```cpp
S vertex_value(int vertex);
S component_sum(int vertex);
```

現在の頂点値、または現在の成分の集約値を返す。
和のモノイドと長さ付きのアフィン作用を渡した場合、`component_sum` は成分の頂点値の総和となる。

## DSU の情報

```cpp
int leader(int vertex);
bool same(int first, int second);
int component_size(int vertex);
int size(int vertex);
int size() const;
int groups() const;
int history_node_count() const;
```

成分の代表元、同じ成分かどうか、成分の頂点数（`size(vertex)` も同じ）、全頂点数、成分数を返す。
`history_node_count` は、初期頂点と成功した成分の併合で作った履歴木のノードの総数を返す。

## 使用例

```cpp
struct Affine { long long multiplier, addend; };
// composition(newer, older) は newer(older(x)) を返すように定義する。
IncrementalComponentAffine<sum_affine_action, 1000> graph(values);
graph.apply_component(0, Affine{2, 1});
graph.add_edge(0, 1);
auto value = graph.vertex_value(0);
auto sum = graph.component_sum(1);
```

## 時間計算量

頂点数を $N$、逆アッカーマン関数を $\alpha$ とする。

| API | 時間計算量（償却） | 追加空間計算量 |
| --- | --- | --- |
| 各コンストラクタ | $O(N)$（固定長版は配列の初期化に $O(\texttt{MAX_SIZE})$） | $O(1)$ |
| `add_edge` | $O(\log N)$ | $O(\log N)$ |
| `apply_component`, `vertex_value` | $O(\log N)$ | $O(\log N)$ |
| `component_sum`, `leader`, `same`, 成分サイズ | $O(\alpha(N))$ | $O(\log N)$ |
| `size()`, `groups`, `history_node_count` | $O(1)$ | $O(1)$ |

追加空間は、主に代表探索の再帰スタックである。

## 空間計算量

DSU と、最大 $2N$ ノードの併合履歴木を合わせて $O(N)$（固定長版では $O(\texttt{MAX_SIZE})$）である。

## 注意点

- 頂点数 `n` が負または `MAX_SIZE` を超える場合、`int` 版のコンストラクタは `std::length_error` を送出する。`values` 版のコンストラクタで `values.size()` が上限を超える場合は `std::runtime_error` を送出する。
- 頂点引数は $[0, n)$ でなければならない。範囲外なら `std::runtime_error` を送出する。
- 辺の削除、頂点の追加、頂点値への個別の加算、過去の状態への問い合わせは行えない。
- 成分は頂点の順序を持たないため、`op` は可換でなければならない。作用の合成は可換でなくてもよい。
- 作用後の値と成分の集約値が型 `S` で表現できることは、呼び出し側の前提である。
