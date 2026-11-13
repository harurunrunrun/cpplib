---
title: Unrooted Tree Isomorphism Test (無根木同型判定) [TREEISO]
documentation_of: ../../../../src/algorithm/tree/isomorphism/are_unrooted_trees_isomorphic.hpp
---

2 本の無根の無向木が、頂点番号の付け替えによって一致するかを判定する。

## API

```cpp
bool are_unrooted_trees_isomorphic(
    const std::vector<std::vector<int>>& first,
    const std::vector<std::vector<int>>& second
);
```

内部では `tree_isomorphic(first, second)` を呼ぶ。

## 引数

- `first`, `second`: 無向木の隣接リスト。

## 戻り値

2 本の木が同型なら `true`、そうでなければ `false` を返す。頂点数が異なる場合も `false` を返す。2 本とも空なら `true` を返す。

## API 別の時間計算量・空間計算量

両方の頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `are_unrooted_trees_isomorphic` | $O(N)$ | $O(N)$ |

## 注意点

- 各隣接リストは、範囲内の頂点番号を持つ単純な無向木でなければならない。違反時は `std::runtime_error` を送出する。
- 隣接リスト内の順序には依存しない。
