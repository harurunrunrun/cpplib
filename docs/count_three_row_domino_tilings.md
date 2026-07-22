---
title: Three-Row Domino Tilings (3行ドミノ敷き詰め数) [M3TILE]
documentation_of: ../src/algorithm/other/dynamic_programming/count_three_row_domino_tilings.hpp
---

3行の長方形を $1\times2$ ドミノで敷き詰める方法数を返す。

## count_three_row_domino_tilings

```cpp
uint64_t count_three_row_domino_tilings(int width);
```

幅 `width` の盤面を隙間なく覆う方法数を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_three_row_domino_tilings` | $O(W)$ | $O(1)$ |

## 注意点

- 負の幅は `runtime_error`、答えの範囲違反は `overflow_error` となる。
