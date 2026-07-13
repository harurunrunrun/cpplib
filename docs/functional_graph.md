---
title: Functional Graph (関数グラフ)
documentation_of: ../src/algorithm/graph/functional_graph.hpp
---

各頂点からちょうど1本の有向辺が出るfunctional graphを、cycleとcycleへ向かう木に分解する。

# 構築

```cpp
FunctionalGraph<MAX_SIZE> graph(successor)
graph.build(successor)
```

`successor[v]` を $v$ から出る辺の行き先とする。`vector<int>` と `array<int, N>` を渡せる。頂点数は `MAX_SIZE` 以下でなければならない。

64段のdoubling tableを構築するため、前処理は $O(64N)$ 時間、$O(64\,\mathrm{MAX\_SIZE})$ 空間を使う。cycle分解自体は $O(N)$ 時間である。

# 基本操作

```cpp
int graph.size()
bool graph.empty()
int graph.component_count()
int graph.successor(v)
```

- `size`, `empty`, `component_count`, `successor`: $O(1)$

# 分解結果

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

1つの弱連結成分にはcycleがちょうど1つ存在するため、`component_id(v)` と `cycle_id(v)` は同じ値を返す。

`cycle_entry(v)` は $v$ から進んで最初に到達するcycle上の頂点、`distance_to_cycle(v)` はそこまでの辺数を返す。`cycle_position(v)` はcycle上の頂点ならcycle内の位置、cycle外なら `-1` を返す。位置は `successor` の向きに1ずつ増え、末尾の次は0になる。`cycle_length(v)` は属するcycleの長さ、`orbit_size(v)` は初めて頂点が重複するまでに通る頂点数を返す。

- 上記の全操作: $O(1)$

# componentとcycle

```cpp
int graph.component_size(id)
int graph.cycle_size(id)
span<const int> graph.cycle(id)
```

`cycle(id)` はcycle上の頂点を辺の向きに並べたviewを返す。末尾の頂点の行き先は先頭の頂点である。

- `component_size`, `cycle_size`: $O(1)$
- `cycle`: $O(1)$、返すviewの長さはcycle長

# 移動

```cpp
int graph.jump(v, unsigned long long k)
long long graph.steps_to(from, to)
```

`jump(v, k)` は $v$ から $k$ 回辺を進んだ頂点を返す。`k` は `unsigned long long` の全範囲を扱える。

`steps_to(from, to)` は `from` から辺を進んで初めて `to` に到達するまでの辺数を返す。到達不能なら `-1` を返す。

- `jump`: $O(64)$
- `steps_to`: $O(64)$
## 注意点

- default constructorは頂点数0の空graph。`build(successor)` は以前の内容を破棄して
  再構築する。頂点数超過、または行き先が $[0,N)$ 外なら例外。
- 頂点を受け取る全queryは $0\le v<size()$、component queryは
  $0\le id<component_count()$ が必要で、違反時は例外。
- `component_size(id)` はcycleへ流入する木も含む弱連結成分の頂点数。
  `cycle_size(id)` はそのうちcycle上の頂点数。
- `cycle(id)` が返すspanはobject内部配列を参照し、次の `build` まで有効。

### 補足

default constructorは空graphを作り、`build` は以前の状態を置換する。入力sizeは `MAX_SIZE` 以下。頂点・component queryはindexを検査する。`cycle` のspanは次のbuildまたはobject破棄まで有効。構築は $O(64N)$、`jump/steps_to` は $O(64)$。

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| constructor / `build(successor)` | $O(64N)$ | object内に $O(64\,MAX\_SIZE)$ |
| `size`, `empty`, `component_count`, `successor` | $O(1)$ | $O(1)$ |
| 頂点ごとの分解結果query | $O(1)$ | $O(1)$ |
| `component_size`, `cycle_size`, `cycle` | $O(1)$ | $O(1)$（`cycle` はview） |
| `jump`, `steps_to` | $O(64)$ | $O(1)$ |
