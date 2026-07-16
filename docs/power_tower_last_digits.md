---
title: Power Tower Last Digits (累乗塔の末尾桁) [POWTOW]
documentation_of: ../src/algorithm/math/power_tower_last_digits.hpp
---

同じ底を右結合で繰り返す累乗塔について、9桁超過の有無と末尾9桁を求める。

## `PowerTowerLastNine`

```cpp
struct PowerTowerLastNine {
    uint32_t value;
    bool truncated;
    string to_string() const;
};
```

`value` は末尾9桁、`truncated` は真の値が9桁を超えるかを表す。`to_string()` は超過時に `...` と0埋め9桁を返す。

## `power_tower_last_nine`

```cpp
PowerTowerLastNine power_tower_last_nine(uint64_t base, uint64_t height);
```

`base ↑↑ height` を処理する。高さ0の値は1。

## 引数

- `base`: 累乗塔で繰り返す非負整数の底。
- `height`: 累乗塔の高さ。

## 戻り値

末尾9桁と、真の値が $10^9$ 以上かを表す `PowerTowerLastNine`。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `PowerTowerLastNine::to_string` | $O(1)$ | $O(1)$ |
| `power_tower_last_nine` | 法 `10^9` の totient 鎖長とその素因数試し割り時間 | $O(\log 10^9)$ |

## 注意点

- `0 ↑↑ 0 = 1` と定義する。
- 底が0の場合、高さが正の偶数なら1、奇数なら0となる。
