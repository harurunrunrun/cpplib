---
title: Burn Bury
documentation_of: ../src/algorithm/matching/burn_bury.hpp
---

燃やす埋める。

二値変数のsubmodularなコストを最小カットで最小化する。

# コンストラクタ

```cpp
BurnBury<T> graph(n)
```

# 関数

```cpp
graph.add_one_cost(v, zero_cost, one_cost)
graph.add_pair_cost(i, j, cost00, cost01, cost10, cost11)
graph.solve()
```

`add_pair_cost` は

```text
cost00 + cost11 <= cost01 + cost10
```

を満たす必要がある。

`BurnBuryResult<T>` は次を持つ。

```cpp
T cost;
vector<int> assignment;
```

`assignment[v]` は `0` または `1`。

## 時間計算量

$V'=N+2$ をflow networkの頂点数、$E'$ を追加済みflow辺数とする。

- `BurnBury(n)`: $O(N)$
- `add_one_delta`, `add_one_cost`: 償却 $O(1)$
- `add_pair_cost`: 償却 $O(1)$（flow辺を高々3本追加）
- `solve`: Dinic法 $O(V'^2E')$、min-cutと長さ $N$ の割当出力 $O(V'+E')$

## 空間計算量

- flow graph、min-cut、戻り値を含めて $O(V'+E')$

## API契約・前提・例外

`add_one_delta(v,d)` はvが1のときだけdを加える。`add_one_cost(v,c0,c1)` は定数c0とdelta `c1-c0` に等価。index違反または非submodularなpair costで `runtime_error`。solveは最小costと0/1割当を返す。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error`。容量・cost・総和の演算結果が `T` に収まることを前提とする。記載した計算量には引数検査とResultの構築を含む。
