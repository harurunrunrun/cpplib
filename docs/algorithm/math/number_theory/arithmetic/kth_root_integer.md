---
title: K-th Root Integer (整数のK乗根) [kth_root_integer]
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/kth_root_integer.hpp
---

符号なし 64 bit 整数の整数 $k$ 乗根を求める。

## `kth_root_integer`

```cpp
uint64_t math::kth_root_integer(uint64_t value, unsigned int exponent);
```

`value` を $v$、`exponent` を $k$ として、$r^k\le v$ を満たす最大の整数 $r$、すなわち $\lfloor v^{1/k}\rfloor$ を返す。

`exponent > 0` を要求し、0 の場合は `invalid_argument` を送出する。累乗の比較は 128 bit 整数で上限を監視しながら行い、64 bit の積をオーバーフローさせない。

## 時間計算量

$B$ を `bit_width(value)` とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 1 回の累乗比較 | $O(\log(k+1))$ | $O(1)$ |
| `kth_root_integer` | $O(\lceil B/k\rceil\log(k+1))$ | $O(1)$ |

`exponent == 1` および `exponent >= 64` は定数時間で処理する。

## 注意点

- `exponent == 0` は `invalid_argument` である。それ以外の入力（`value == 0` を含む）では例外を送出しない。
