---
title: Convolution Modulo 2^64 (2の64乗を法とする畳み込み) [convolution_mod_2_64]
documentation_of: ../../../../src/algorithm/fft/transform/convolution_mod_2_64.hpp
---

## 対応上限

```cpp
inline constexpr size_t convolution_mod_2_64_max_input_size;
```

各入力列について保証する最大長 `524288` を表す。

## 畳み込み

```cpp
vector<uint64_t> convolution_mod_2_64(
    const vector<uint64_t>& first,
    const vector<uint64_t>& second
);
```

$$
c_k=\sum_{i+j=k}\mathtt{first}[i]\mathtt{second}[j]\pmod{2^{64}}
$$

を返す。5個の約30 bit NTT素数で整数としての係数を一意に復元し、
最後にunsigned 64 bitの下位だけを残す。
各入力長が上限以下なら、復元用の法の積は
整数係数の最大値より大きい。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（戻り値を含む） |
| --- | --- | --- |
| `convolution_mod_2_64_max_input_size` | $O(1)$ | $O(1)$ |
| `convolution_mod_2_64(first, second)` | $O(Z\log Z)$ | $O(Z)$ |

$Z$ は `first.size() + second.size() - 1` 以上の最小の2冪である。
定数倍として5組のNTTを行う。

## 注意点

- いずれかの入力が空なら空列を返す。
- 上限を超える入力では `std::length_error` を送出する。
- 加減乗算のwraparoundは `uint64_t` の規定された剰余演算を使う。
- 5法CRTは上限を含めた整数係数の範囲を復元するためのもので、
  3法CRTによる任意mod畳み込みとは異なる。
