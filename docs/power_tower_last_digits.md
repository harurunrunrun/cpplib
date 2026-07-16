---
title: Power Tower Last Digits (累乗塔の末尾桁) [POWTOW]
documentation_of: ../src/algorithm/math/power_tower_last_digits.hpp
---

同じ底を右結合で繰り返す累乗塔について、指定桁数超過の有無と末尾桁を求める。

## `TetrationLastDigits`

```cpp
struct TetrationLastDigits {
    uint64_t value;
    unsigned digit_count;
    bool truncated;
    string to_string() const;
};
```

`value` は末尾 `digit_count` 桁、`truncated` は真の値が
$10^{\mathrm{digit\_count}}$ 以上かを表す。`to_string()` は超過時に
`...` と0埋めした末尾桁を返す。

## `tetration_last_digits`

```cpp
TetrationLastDigits tetration_last_digits(
    uint64_t base,
    uint64_t height,
    unsigned digit_count
);
```

`base ↑↑ height` を処理する。高さ0の値は1である。

## 引数

- `base`: 累乗塔で繰り返す非負整数の底。
- `height`: 累乗塔の高さ。
- `digit_count`: 取得する10進末尾桁数。$1$ 以上 $19$ 以下。

## 戻り値

指定した末尾桁と、真の値がその桁数に収まるかを表す
`TetrationLastDigits`。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `TetrationLastDigits::to_string` | $O(D)$ | $O(D)$ |
| `tetration_last_digits` | $O(D^2)$ | $O(D)$ |

## 注意点

- `0 ↑↑ 0 = 1` と定義する。
- 底が0の場合、高さが正の偶数なら1、奇数なら0となる。
- $D$ は `digit_count` である。
- 桁数が範囲外なら `invalid_argument` を送出する。
