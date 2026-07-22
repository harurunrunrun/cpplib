---
title: K-th Root Integer (整数のK乗根) [kth_root_integer]
documentation_of: ../src/algorithm/math/number_theory/kth_root_integer.hpp
---

符号なし64bit整数の整数k乗根を求める。

## `kth_root_integer`

```cpp
uint64_t math::kth_root_integer(uint64_t value, unsigned int exponent);
```

最大の整数$r$で$r^{exponent}\le value$を満たすもの、すなわち
$\lfloor value^{1/exponent}\rfloor$を返す。

`exponent > 0`を要求する。0の場合は`invalid_argument`を送出する。
累乗比較は128bit整数で上限を監視し、64bitの積をoverflowさせない。

`B = bit_width(value)`とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 1回の累乗比較 | $O(\log(exponent+1))$ | $O(1)$ |
| `kth_root_integer` | $O(\lceil B/exponent\rceil\log(exponent+1))$ | $O(1)$ |

`exponent == 1`および`exponent >= 64`は定数時間で処理する。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
