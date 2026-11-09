---
title: Functional Graph (関数グラフ)
documentation_of: ../../../../src/algorithm/graph/traversal/functional_graph.hpp
---

各頂点からちょうど 1 本の有向辺が出る関数グラフ（functional graph）を、閉路と閉路へ向かう木に分解する。

## 構築

```cpp
FunctionalGraph<MAX_SIZE> graph(successor)
graph.build(successor)
```

`successor[v]` を $v$ から出る辺の行き先とする。`vector<int>` と `array<int, N>` を渡せる。`MAX_SIZE` を指定した場合、頂点数は `MAX_SIZE` 以下でなければならない。

テンプレート引数を省略した `FunctionalGraph<>` は、実行時のサイズに合わせた `std::vector` を使う可変長版である。容量を明示する固定長版も同じヘッダにある。

```cpp
FunctionalGraph<> dynamic(successors);
FunctionalGraph<100000> fixed(successors);
```

64 段のダブリング表を構築するため、前処理は $O(64N)$ 時間を使う。表の領域は可変長版で $O(64N)$、固定長版ではオブジェクト内に $O(64\,\texttt{MAX_SIZE})$ である。64 は移動回数に使う `unsigned long long` のビット幅であり、頂点の容量とは独立している。閉路への分解自体は $O(N)$ 時間である。

デフォルトコンストラクタは頂点数 0 の空のグラフを作る。`build(successor)` は以前の内容を破棄して再構築する。

## 基本操作

```cpp
int graph.size()
bool graph.empty()
int graph.component_count()
int graph.successor(v)
```

- `size`, `empty`, `component_count`, `successor`: $O(1)$

## 分解結果

```cpp
int graph.component_id(v)
int graph.cycle_id(v)
bool graph.on_cycle(v)
int graph.distance_to_cycle(v)
int graph.cycle_entry(v)
int graph.cycle_position(v)
int graph.cycle_length(v)
int graph.orbit_size(v)
```

1 つの弱連結成分には閉路がちょうど 1 つ存在するため、`component_id(v)` と `cycle_id(v)` は同じ値を返す。

`cycle_entry(v)` は $v$ から進んで最初に到達する閉路上の頂点、`distance_to_cycle(v)` はそこまでの辺数を返す。`cycle_position(v)` は、閉路上の頂点なら閉路内での位置、閉路外なら `-1` を返す。位置は `successor` の向きに 1 ずつ増え、末尾の次は 0 に戻る。`cycle_length(v)` は $v$ が流れ込む閉路の長さ、`orbit_size(v)` は $v$ から進んで初めて頂点が重複するまでに通る頂点数を返す。

- 上記の全操作: $O(1)$

## 成分と閉路

```cpp
int graph.component_size(id)
int graph.cycle_size(id)
span<const int> graph.cycle(id)
```

`component_size(id)` は、閉路へ流入する木も含めた弱連結成分の頂点数を返す。`cycle_size(id)` はそのうち閉路上の頂点数を返す。

`cycle(id)` は閉路上の頂点を辺の向きに並べたビューを返す。末尾の頂点の行き先は先頭の頂点である。返す `span` はオブジェクト内部の配列を参照し、次の `build` またはオブジェクトの破棄まで有効である。

- `component_size`, `cycle_size`: $O(1)$
- `cycle`: $O(1)$（返すビューの長さは閉路の長さ）

## 移動

```cpp
int graph.jump(v, unsigned long long k)
long long graph.steps_to(from, to)
```

`jump(v, k)` は $v$ から $k$ 回辺を進んだ頂点を返す。`k` は `unsigned long long` の全範囲を扱える。

`steps_to(from, to)` は、`from` から辺を進んで初めて `to` に到達するまでの辺数を返す。到達できなければ `-1` を返す。

- `jump`: $O(64)$
- `steps_to`: $O(64)$

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ / `build(successor)` | $O(64N)$ | 可変長版 $O(64N)$、固定長版はオブジェクト内に $O(64\,\texttt{MAX_SIZE})$ |
| `size`, `empty`, `component_count`, `successor` | $O(1)$ | $O(1)$ |
| 頂点ごとの分解結果のクエリ | $O(1)$ | $O(1)$ |
| `component_size`, `cycle_size`, `cycle` | $O(1)$ | $O(1)$（`cycle` はビュー） |
| `jump`, `steps_to` | $O(64)$ | $O(1)$ |

## 注意点

- 頂点数が `MAX_SIZE` を超える場合、または行き先が $[0,N)$ の外にある場合は `runtime_error` を送出する。
- 頂点を受け取るクエリはすべて $0\le v<N$、成分を受け取るクエリは $0\le \mathit{id}<$ `component_count()` が必要で、違反時は `runtime_error` を送出する。
