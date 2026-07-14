---
title: Maximum Labyrinth Rope Length (迷宮内の最大ロープ長) [LABYR1]
documentation_of: ../src/algorithm/graph/maximum_labyrinth_rope_length.hpp
---

通行可能マス間の経路が一意である迷宮について、配置できるロープの最大長を返す。

# maximum_labyrinth_rope_length

```cpp
int maximum_labyrinth_rope_length(const vector<string>& labyrinth);
```

`.` を通行可能マス、`#` を壁とする4近傍格子の各連結成分の直径を求め、その最大値を辺数で返す。通行可能マスが存在しない場合は `0` を返す。

## API別の時間計算量・空間計算量

行数を $R$、列数を $C$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_labyrinth_rope_length` | $O(RC)$ | $O(RC)$ |

## 注意点

- 各通行可能連結成分では、任意の2マス間の単純路が一意でなければならない。
- 全行の長さは等しく、各文字は `.` または `#` でなければならない。
- 行長不一致または不正文字の場合は `runtime_error` を送出する。
