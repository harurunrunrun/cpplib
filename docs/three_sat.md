---
title: Three Sat
documentation_of: ../src/algorithm/graph/three_sat.hpp
---

3-SAT。

# 関数

```cpp
three_sat(n, clauses)
```

`clauses` は `ThreeSatClause` の列。各節は3つの `ThreeSatLiteral{variable, value}` を持つ。

戻り値 `ThreeSatResult` は次を持つ。

```cpp
bool satisfiable;
vector<int> assignment;
```

充足不能なら `assignment` は空。

## 時間計算量

$N$ を変数数、$M$ を節数とする。

- `three_sat`: worst-case $O(2^N N(M+1))$

各探索nodeでunit propagationと分岐変数選択に $O(NM+N)$ を使う。充足可能な場合の長さ $N$ の割当出力もこの範囲に含む。

## 空間計算量

- 再帰ごとの割当コピーと戻り値を含めて $O(N^2+M)$
