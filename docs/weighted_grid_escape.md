---
title: Weighted Grid Escape (重み付き格子脱出判定) [BYTESE1]
documentation_of: ../src/algorithm/graph/weighted_grid_escape.hpp
---

# weighted_grid_escape

```cpp
optional<long long> weighted_grid_escape(
    const vector<vector<long long>>& cost,
    int target_row,
    int target_column,
    long long budget
)
```

## 引数

- `cost`: 各マスへ入るときに支払う非負費用。左上マスの費用も支払う。
- `target_row`, `target_column`: 0-indexedの目標マス。
- `budget`: 利用できる費用。

## 戻り値

左上から4近傍で目標へ移動する最小費用が `budget` 以下なら残額、超えるなら `nullopt`。

## API別の時間計算量・空間計算量

マス数を $V$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `weighted_grid_escape` | $O(V\log(V+1))$ | $O(V)$ |

## 注意点

- `cost` は空でない長方形で、各費用は非負、目標は範囲内でなければならない。
- 前提違反、マス数または距離のoverflowでは対応する標準例外を送出する。
