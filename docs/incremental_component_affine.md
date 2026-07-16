---
title: Incremental Component Affine (増分連結成分アフィン作用)
documentation_of: ../src/structure/dsu/incremental_component_affine.hpp
---

頂点値を持つ増分無向グラフについて、辺追加、連結成分全体へのアフィン作用、頂点値と成分和を処理する。
成分の合併履歴を木にし、作用を頂点から現在の履歴根まで順序付きで合成する。
したがって、合併前の作用が後から連結された頂点へ適用されることはない。

## テンプレート引数

```cpp
IncrementalComponentAffine<MonoidActLen, MAX_SIZE>
```

`MonoidActLen` は `structure/types/monoid_act_len.hpp` と同じ次の要素を持つ。

- 集約値型 `S` と作用型 `T`
- 可換な成分集約 `op(first, first_length, second, second_length)` と単位元 `e()`
- `mapping(action, aggregate, length)`
- `composition(newer, older)` と恒等作用 `id()`

アフィン作用 `older` の後に `newer` を適用する合成は `composition(newer,older)` である。
`S` と `T` はデフォルト構築、コピー、代入ができる必要がある。

## コンストラクタ

```cpp
IncrementalComponentAffine(int n = MAX_SIZE);
IncrementalComponentAffine(const vector<S>& values);
```

`n` 個の単独頂点を値 `e()` で作る、または `values[v]` を頂点初期値として作る。

## `add_edge`

```cpp
bool add_edge(int first, int second);
```

無向辺を追加する。異なる成分を併合した場合だけ `true` を返す。
自己ループと同じ成分内の辺は状態を変えず `false` を返す。

## `apply_component`

```cpp
void apply_component(int vertex, const T& action);
```

`vertex` を含む現在の連結成分に属する全頂点へ `action` を適用する。
後から辺で加わる頂点にはこの作用を遡って適用しない。

## `vertex_value` / `component_sum`

```cpp
S vertex_value(int vertex);
S component_sum(int vertex);
```

現在の頂点値、または現在の成分集約値を返す。
和モノイドと長さ付きアフィン作用を渡した場合、`component_sum` は成分の頂点値総和となる。

## DSU情報API

```cpp
int leader(int vertex);
bool same(int first, int second);
int component_size(int vertex);
int size(int vertex);
int size() const;
int groups() const;
int history_node_count() const;
```

成分代表、同一成分判定、成分サイズ、全頂点数、成分数を返す。
`history_node_count` は初期頂点と成功した成分併合で作った履歴節点の総数を返す。

## 使用例

```cpp
struct Affine { long long multiplier, addend; };
// composition(newer, older) は newer(older(x)) を返すよう定義する。
IncrementalComponentAffine<sum_affine_action, 1000> graph(values);
graph.apply_component(0, Affine{2, 1});
graph.add_edge(0, 1);
auto value = graph.vertex_value(0);
auto sum = graph.component_sum(1);
```

## API別の時間計算量・空間計算量

頂点数を $N$、逆Ackermann関数を $\alpha$ とする。

| API | 償却時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 各コンストラクタ | $O(\mathtt{MAX\_SIZE})$ | $O(1)$ |
| `add_edge` | $O(\log N+\alpha(N))$ | $O(1)$ |
| `apply_component`, `vertex_value` | $O(\log N+\alpha(N))$ | $O(1)$ |
| `component_sum`, `leader`, `same`, 成分サイズ | $O(\alpha(N))$ | $O(\log N)$ |
| `size()`, `groups`, `history_node_count` | $O(1)$ | $O(1)$ |

本体の保存領域はDSUと最大 `2*MAX_SIZE` 節点の合併履歴木を合わせて $O(\mathtt{MAX\_SIZE})$ である。

## 注意点

- `0 <= n <= MAX_SIZE`、頂点引数は `[0,n)`。範囲外なら `runtime_error` を送出する。
- 辺の削除、頂点の追加、頂点値への個別加算、過去状態への問い合わせは行えない。
- 成分は頂点の順序を持たないため、`op` は可換でなければならない。作用の合成は可換でなくてもよい。
- 作用後の値と成分集約が型 `S` で表現できることは呼び出し側の前提である。
