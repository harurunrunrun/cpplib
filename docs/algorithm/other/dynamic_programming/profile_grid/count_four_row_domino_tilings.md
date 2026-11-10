---
title: Four-Row Domino Tilings (4行ドミノ敷き詰め数) [M4TILE]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/profile_grid/count_four_row_domino_tilings.hpp
---

高さ $4$ の長方形を $1\times2$ のドミノで敷き詰める方法の数を、列ごとのビット DP で求める。

## count_four_row_domino_tilings

```cpp
std::uint64_t count_four_row_domino_tilings(int width);
```

幅 `width` の盤面を隙間なく覆う方法の数を返す。`width == 0` では $1$ を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_four_row_domino_tilings` | $O(W)$ | $O(1)$ |

## 注意点

- 負の幅では `std::runtime_error` を送出する。
- 途中の値が `std::uint64_t` に収まらない場合は `std::overflow_error` を送出する。
