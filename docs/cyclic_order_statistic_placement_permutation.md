---
title: Cyclic Order-statistic Placement Permutation (巡回順序統計配置順列) [CTRICK]
documentation_of: ../src/algorithm/other/cyclic_order_statistic_placement_permutation.hpp
---

空き位置を巡回し、空き位置の順位で配置先を選んで得られる順列を構築します。

## API

### `cyclic_order_statistic_placement_permutation(size)`

```cpp
template<int MAX_SIZE = 20000>
vector<int> cyclic_order_statistic_placement_permutation(int size);
```

値 `value = 1, 2, ..., size` を順に配置します。最初の順位を0とし、各回で
`rank = (rank + value) % remaining` と更新して、0始まりで `rank` 番目の
空き位置へ `value` を置きます。戻り値は位置順の配置結果です。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `cyclic_order_statistic_placement_permutation` | $O(size\log MAX\_SIZE)$ | $O(size\log MAX\_SIZE)$ |

## 注意点

- `0 <= size <= MAX_SIZE` が必要です。
- `MAX_SIZE` は正でなければなりません。
- `size` が範囲外なら `out_of_range`、内部の順序統計取得に失敗した場合は
  `runtime_error` を送出します。
