---
title: 2-SAT (2充足可能性問題)
documentation_of: ../../../../../src/algorithm/graph/constraints/logical/two_sat.hpp
---

2-SAT を含意グラフの強連結成分分解で解く。

## コンストラクタ

```cpp
TwoSat ts(n)
```

変数 $x_0, \ldots, x_{n-1}$ をもつ 2-SAT を作る。$n < 0$ なら `runtime_error` を送出する。

## メンバ関数

```cpp
int ts.id(int variable, bool value) const
void ts.add_implication(int i, bool f, int j, bool g)
void ts.add_clause(int i, bool f, int j, bool g)
bool ts.satisfiable()
```

- `id(variable, value)`: リテラル $x_{\mathrm{variable}} = \mathrm{value}$ に対応する含意グラフの頂点番号 `2 * variable + value` を返す。
- `add_implication(i, f, j, g)`: 含意 $(x_i = f) \Rightarrow (x_j = g)$ を追加する（対偶の辺も同時に追加する）。
- `add_clause(i, f, j, g)`: 節 $(x_i = f) \lor (x_j = g)$ を追加する。
- `satisfiable()`: 充足可能なら `true` を返し、公開メンバ `answer` に充足割当（`answer[v]` が 0 または 1）を書き込む。充足不可能なら `false` を返す（このとき `answer` は途中まで更新されている可能性がある）。

## 時間計算量

$N$ を変数の個数、$M$ を現在の含意辺の本数とする。

- `TwoSat(n)`: $O(N)$
- `id`: $O(1)$
- `add_implication`: 償却 $O(1)$
- `add_clause`: 償却 $O(1)$（含意辺を 2 本追加する）
- `satisfiable`: $O(N + M)$（強連結成分分解と長さ $N$ の `answer` の更新を含む）

## 空間計算量

含意グラフと割当を含めて $O(N + M)$。

## 注意点

- 変数番号が $[0, N)$ の範囲外なら `runtime_error` を送出する。
- `satisfiable` 内の強連結成分分解は再帰 DFS で実装されているため、含意グラフが大きいとスタックが深くなる。
