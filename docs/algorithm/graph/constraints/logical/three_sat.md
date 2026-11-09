---
title: 3-SAT (3充足可能性問題)
documentation_of: ../../../../../src/algorithm/graph/constraints/logical/three_sat.hpp
---

各節がちょうど 3 個のリテラルからなる CNF の充足可能性を、単位伝播付きの分岐探索（DPLL）で判定する。分岐には、未充足の節に最も多く現れる未割当変数を選ぶ。

## 関数

```cpp
ThreeSatResult three_sat(int n, const vector<ThreeSatClause>& clauses)
```

`clauses` は `ThreeSatClause` の列。各節は 3 つの `ThreeSatLiteral{variable, value}` を `literal` に持ち、リテラルは「変数 `variable` の値が `value` である」ことを表す。

戻り値の `ThreeSatResult` は次のメンバを持つ。

```cpp
bool satisfiable;
vector<int> assignment;
```

充足可能なら `assignment[i]` は変数 $i$ の値（0 または 1）である。充足不能なら `assignment` は空。

## 時間計算量

$N$ を変数の個数、$M$ を節の個数とする。

- `three_sat`: 最悪 $O(2^N N (M + 1))$

各探索ノードで、単位伝播と分岐変数の選択に $O(NM + N)$ を使う。充足可能な場合に長さ $N$ の割当を出力する時間もこの範囲に含む。

## 空間計算量

- 再帰ごとの割当のコピーと戻り値を含めて $O(N^2 + M)$

## 注意点

`n` が負、またはリテラルの変数番号が $[0, n)$ の範囲外の場合は `std::runtime_error` を送出する。記載した計算量には引数の検査と戻り値の構築を含む。探索は再帰で行い、再帰の深さは最大 $N$ である。
