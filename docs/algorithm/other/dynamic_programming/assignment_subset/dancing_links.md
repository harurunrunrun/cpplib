---
title: Dancing Links (Dancing Links 法)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/assignment_subset/dancing_links.hpp
---

疎な完全被覆（exact cover）の行列を双方向循環リストで保持し、Algorithm X における列の選択・復元を定数時間のリンク操作で行う。分岐には、残っている行が最も少ない列を選ぶ。

## API

```cpp
DancingLinks solver(column_count);
```

すべての列が一次列（ちょうど 1 回覆う必要がある列）である、行のない完全被覆問題を構築する。`column_count` が負なら `std::runtime_error` を送出する。

```cpp
int row_id = solver.add_row(columns);
```

`columns` で指定した列（0-indexed）に $1$ をもつ行を追加し、0-indexed の行番号を返す。

```cpp
std::vector<std::vector<int>> solutions = solver.solve(solution_limit = 1);
```

高々 `solution_limit` 個の解を、選んだ行番号の列として返す。`solution_limit == 0` なら探索せずに空を返す。同じオブジェクトで再実行できる。

## 時間計算量

- 構築: $O(C)$（$C$ は列数）
- `add_row`: 行の非零要素数を $k$ として $O(k \log k)$（重複検査のためのソートを含む）
- `solve`: 探索木の訪問節点数を $T$、各分岐で削除・復元する非零要素の総数を $Z_T$ として $O(T+Z_T)$。ただし各節点で分岐列の選択に $O(C)$ かかる。一般の完全被覆問題は NP 完全であり、最悪で指数時間

## 空間計算量

入力の非零要素数を $Z$ として $O(C+Z)$ に加え、返す解の大きさ。

## 注意点

- 行内の列番号は範囲内かつ重複なしでなければならない。違反時は `std::runtime_error` を送出する。
- 空行はどの列も覆わないため、解には採用されない。
