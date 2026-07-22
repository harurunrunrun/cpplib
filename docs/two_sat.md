---
title: 2-SAT (2充足可能性問題)
documentation_of: ../src/algorithm/graph/constraints/two_sat.hpp
---

2-SAT。

## コンストラクタ

```cpp
TwoSat ts(n)
```

## 関数

```cpp
ts.add_clause(i, f, j, g)
ts.satisfiable()
```

`add_clause(i, f, j, g)` は `(x_i == f) OR (x_j == g)` を追加する。

`satisfiable()` が `true` の場合、`ts.answer` に割当が入る。

## 時間計算量

$N$ を変数数、$M$ を現在の含意辺数とする。

- `TwoSat(n)`: $O(N)$
- `id`: $O(1)$
- `add_implication`: 償却 $O(1)$
- `add_clause`: 償却 $O(1)$（含意辺を2本追加）
- `satisfiable`: $O(N+M)$

`satisfiable` はSCC分解と長さ $N$ の `answer` 更新を含む。

## 空間計算量

- 含意グラフと割当を含めて $O(N+M)$

## 注意点

`id(variable,value)` はliteralに対応する含意graph頂点を返す。`add_implication(i,f,j,g)` は `(x_i==f) => (x_j==g)` を追加する。`satisfiable()` がtrueならpublic `answer[v]` が0/1の充足割当。変数範囲違反で `runtime_error`。

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
