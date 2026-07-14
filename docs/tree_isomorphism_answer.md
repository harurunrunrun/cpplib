---
title: Tree Isomorphism Answer (木同型判定回答) [TREEISO]
documentation_of: ../src/algorithm/tree/tree_isomorphism_answer.hpp
---

2本の無根無向木が同型かをboolで返す回答APIである。

## API

### `tree_isomorphism_answer(first, second)`

隣接listで与えた2本の木が同型なら `true` を返す。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$

## 注意点

- 両方が同じ頂点数の単純な木でなければならない。
- 頂点番号は各隣接listの範囲内でなければならない。不正入力では `runtime_error`。
