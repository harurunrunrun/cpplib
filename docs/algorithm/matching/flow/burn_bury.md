---
title: Binary Submodular Optimization (燃やす埋める)
documentation_of: ../../../../src/algorithm/matching/flow/burn_bury.hpp
---

燃やす埋める問題を解く。二値変数についての劣モジュラなコストの和を、最小カットにより最小化する。

## コンストラクタ

```cpp
BurnBury<T> graph(n)
```

$n$ 個の二値変数を持つ問題を作る。

## メンバ関数

```cpp
graph.add_one_cost(v, zero_cost, one_cost)
graph.add_one_delta(v, delta)
graph.add_pair_cost(i, j, cost00, cost01, cost10, cost11)
graph.solve()
```

- `add_one_cost(v, zero_cost, one_cost)`: 変数 `v` が $0$ のとき `zero_cost`、$1$ のとき `one_cost` を加える。定数 `zero_cost` の加算と `add_one_delta(v, one_cost - zero_cost)` に等しい。
- `add_one_delta(v, delta)`: 変数 `v` が $1$ のときだけ `delta` を加える。`delta` は負でもよい。
- `add_pair_cost(i, j, cost00, cost01, cost10, cost11)`: 変数 `i`, `j` の値の組が $(0, 0), (0, 1), (1, 0), (1, 1)$ のとき、それぞれ `cost00`, `cost01`, `cost10`, `cost11` を加える。劣モジュラ性、すなわち次を満たす必要がある。

```text
cost00 + cost11 <= cost01 + cost10
```

- `solve()`: 最小コストと、それを達成する割当を `BurnBuryResult<T>` で返す。

`BurnBuryResult<T>` は次のメンバを持つ。

```cpp
T cost;
vector<int> assignment;
```

`assignment[v]` は `0` または `1` である。

## 時間計算量

フローネットワークの頂点数を $V' = N+2$、追加済みのフロー辺数を $E'$ とする。

- `BurnBury(n)`: $O(N)$
- `add_one_delta`, `add_one_cost`: 償却 $O(1)$
- `add_pair_cost`: 償却 $O(1)$（フロー辺を高々 3 本追加する）
- `solve`: Dinic 法に $O(V'^2 E')$、最小カットと長さ $N$ の割当の出力に $O(V'+E')$

## 空間計算量

- フローグラフ、最小カット、戻り値を含めて $O(V'+E')$

## 注意点

変数の個数が負の場合、添字が範囲外の場合、劣モジュラでないペアコストを与えた場合は `runtime_error` を送出する。コストや流量の総和の演算結果が `T` に収まることを前提とする。記載した計算量には引数検査と戻り値の構築を含む。

`solve` は何度呼んでもよい。内部のフローは残り、2 回目以降は前回からの差分だけを流すが、コストは流量の総和から計算するので、間にコストを追加した場合も含めて正しい結果を返す。
