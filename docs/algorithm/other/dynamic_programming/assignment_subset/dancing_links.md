---
title: Dancing Links (Dancing Links法)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/assignment_subset/dancing_links.hpp
---

疎なexact cover行列を双方向循環リストで保持し、Algorithm Xの列選択・復元を定数時間リンク操作で行う。

## API

```cpp
DancingLinks solver(column_count);
```

すべての列が一次列である空のexact cover問題を構築する。

```cpp
int row_id = solver.add_row(columns);
```

指定列に `1` を持つ行を追加し、0始まりの行番号を返す。

```cpp
vector<vector<int>> solutions = solver.solve(solution_limit = 1);
```

高々 `solution_limit` 個の解を、選択行番号の列として返す。同じオブジェクトで再実行できる。

## 時間計算量

- 構築と `add_row`: 行の非零要素数に比例
- `solve`: 解探索木の訪問節点数を $T$、各分岐で削除・復元する非零要素総数を $Z_T$ として $O(T+Z_T)$。一般のexact coverはNP完全であり最悪指数時間

## 空間計算量

入力の非零要素数を $Z$ として $O(C+Z)$ に加え、返す解の大きさ。

## 注意点

行内の列番号は範囲内かつ重複なしでなければならない。違反時は `runtime_error` を送出する。空行はどの列も覆わないため解には採用されない。
