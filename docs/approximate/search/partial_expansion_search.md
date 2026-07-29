---
title: Partial-Expansion Heuristic Search (部分展開ヒューリスティック探索)
documentation_of: ../../../src/approximate/search/partial_expansion_search.hpp
---

A* の候補を一度にすべて OPEN へ挿入せず、閾値または次の後続だけを扱う探索群。
`neighbors(state)` は `pair<State, Cost>` の range、`goal(state)` は真偽値、
`heuristic(state)` は非負の推定コストを返す。

## `fringe_search`

```cpp
fringe_search(start, goal, neighbors, heuristic, hash = {}, equal = {})
```

優先度付きキューを使わず、現在の $f=g+h$ 上限以下を `current`、超える候補を
`later` に分ける Fringe Search。次の上限は `later` に現れた最小 $f$ とする。
許容的ヒューリスティックの下で最適解を返す。

反復回数を $I$、各反復で走査する状態数と辺数を $X_i,A_i$ とすると、
時間計算量は $O(\sum_i(X_i+A_i))$、空間計算量は $O(V_d+A_d)$。
$V_d,A_d$ は発見した状態・遅延候補の数。

## `partial_expansion_a_star_search`

```cpp
partial_expansion_a_star_search(
    start, goal, neighbors, heuristic, hash = {}, equal = {}
)
```

各状態の後続を $f$ 順に保持し、1 回の部分展開につき未処理の最良後続を
1 個だけ OPEN へ挿入する Partial Expansion A*。未処理後続があれば、その次の
$f$ を親の OPEN キーとして親を再挿入する。許容的ヒューリスティックの下で
最適解を返す。

生成辺数を $A$、部分展開回数を $P$、OPEN 最大要素数を $Q$ とすると、
後続整列を含む時間計算量は
$O(A\log A_{\max}+P\log Q)$、空間計算量は $O(V_d+A+Q)$。
$A_{\max}$ は 1 状態の最大分岐数。

## `pea_star_search`

```cpp
pea_star_search(start, goal, neighbors, heuristic, hash = {}, equal = {})
```

状態が保持する展開上限以下の後続だけを OPEN へ挿入し、超えた後続の最小 $f$ を
次の展開上限として親を再挿入する PEA*。後続列を保持せず、再展開時に
`neighbors` を呼び直すことで OPEN と後続保存量を抑える。許容的
ヒューリスティックの下で最適解を返す。

部分展開回数を $P$、各部分展開で列挙する辺数を $b_i$、OPEN 最大要素数を $Q$
とすると、時間計算量は $O(\sum_i b_i+P\log Q)$、
空間計算量は $O(V_d+Q)$。

## 注意点

- 負辺、負または非有限な推定値には `std::invalid_argument` を送出する。
- 最適性には非負辺と許容的ヒューリスティックが必要。整合的
  ヒューリスティックを用いると再展開を抑えられる。
- `neighbors` は同じ状態に複数回呼ばれ得る。特に `pea_star_search` は
  後続を保存しないため、呼び出しが副作用によって結果を変えてはならない。
- `Cost{}` がゼロを表し、コスト加算と `long double` への変換が表現可能であることを
  前提とする。
