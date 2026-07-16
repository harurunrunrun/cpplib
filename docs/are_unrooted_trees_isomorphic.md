---
title: Unrooted Tree Isomorphism Test (無根木同型判定) [TREEISO]
documentation_of: ../src/algorithm/tree/are_unrooted_trees_isomorphic.hpp
---

2本の無根無向木が、頂点番号の付け替えによって一致するかを判定する。

# are_unrooted_trees_isomorphic

```cpp
bool are_unrooted_trees_isomorphic(
    const vector<vector<int>>& first,
    const vector<vector<int>>& second
);
```

## 引数

- `first`, `second`: 無向木の隣接リスト。

## 戻り値

2本の木が同型なら `true`、そうでなければ `false`。頂点数が異なる場合も `false` を返す。2本とも空なら `true`。

## API別の時間計算量・空間計算量

両方の頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `are_unrooted_trees_isomorphic` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 各隣接リストは、範囲内の頂点番号を持つ単純な無向木でなければならない。違反時は `runtime_error` を送出する。
- 隣接リスト内の順序には依存しない。
