---
title: Normalized Line Result (正規化直線の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/normalized_line_result.hpp
---

直線の方程式の正規化済み係数を保持する値型。

## API

```cpp
struct NormalizedLine {
    long double a;
    long double b;
    long double c;
};
```

各フィールドは直線 $ax + by + c = 0$ の係数を表す。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 構築・フィールドの参照・コピー | $O(1)$ | $O(1)$ |

## 注意点

この型自体は正規化条件を検査しない。`normalized_line` の戻り値として使い、その場合は
$a^2 + b^2 = 1$ かつ「$a > 0$、または $a = 0$ かつ $b > 0$」となるよう符号がそろえられている。
