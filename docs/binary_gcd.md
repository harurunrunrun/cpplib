---
title: Binary GCD (二進GCD)
documentation_of: ../src/algorithm/math/number_theory/binary_gcd.hpp
---

Stein法により固定幅整数の最大公約数を求める。
除算の代わりに末尾0の除去、減算、比較を使う。

## 関数

### `math::binary_gcd(first, second)`

`uint64_t` 2値の非負の最大公約数を返す。

- `binary_gcd(0, 0) == 0`
- 一方だけが0なら他方を返す
- `UINT64_MAX` を含む全 `uint64_t` 入力を扱える
- 例外は送出しない

### `math::binary_gcd_signed(first, second)`

`int64_t` 2値の符号を無視して最大公約数を返す。
戻り値は `uint64_t` である。
INT64_MIN の絶対値 2^63 も uint64_t で表現できる。
`binary_gcd_signed(INT64_MIN, 0) == 9223372036854775808` となる。

## 使用例

```cpp
auto g1 = math::binary_gcd(48, 18);
auto g2 = math::binary_gcd_signed(-48, 18);
```

## API別の時間計算量・空間計算量

M を2入力の絶対値の最大値とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `math::binary_gcd(first,second)` | $O(\log(M+1))$ | $O(1)$ |
| `math::binary_gcd_signed(first,second)` | $O(\log(M+1))$ | $O(1)$ |

## 注意点

- `binary_gcd` は符号なし整数専用。
- signed入力は暗黙変換せず `binary_gcd_signed` に渡す。
- 戻り値は常に非負で、両方0の場合だけ0。
