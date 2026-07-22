---
title: Functional Grid Cycle Count (関数型格子の閉路数) [HERDING]
documentation_of: ../src/algorithm/graph/traversal/functional_grid_cycle_count.hpp
---

各マスが上下左右の隣接マスを1つ指す格子を関数グラフとみなし、その有向閉路数を返す。
関数グラフでは各弱連結成分が閉路をちょうど1つ持つため、戻り値は弱連結成分数とも一致する。

## `functional_grid_cycle_count`

```cpp
size_t functional_grid_cycle_count(
    const vector<string>& directions
);
```

`directions[row][column]` の文字を次のように解釈する。

| 文字 | 次のマス |
| --- | --- |
| `N` | 1行上 |
| `S` | 1行下 |
| `W` | 1列左 |
| `E` | 1列右 |

空の格子と列数0の格子では0を返す。

## API別の時間計算量・空間計算量

行数を $R$、列数を $C$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `functional_grid_cycle_count` | $O(RC)$ | $O(RC)$ |

## 注意点

- 全行の長さは等しくなければならない。
- 各文字は `N`、`S`、`W`、`E` のいずれかで、指すマスが格子内になければならない。
- 不正な形・文字・遷移には `invalid_argument`、要素数の乗算が `size_t` を越える場合は `length_error` を送出する。
