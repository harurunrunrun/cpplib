---
title: Convolution Modulo 2^64 (2の64乗を法とする畳み込み) [convolution_mod_2_64]
documentation_of: ../../../../src/algorithm/fft/transform/convolution_mod_2_64.hpp
---

`std::uint64_t` の列を、法 $2^{64}$ で畳み込む。

## 対応上限

```cpp
inline constexpr std::size_t convolution_mod_2_64_max_input_size = 524288;
```

各入力列について動作を保証する最大長である。

## 畳み込み

```cpp
std::vector<std::uint64_t> convolution_mod_2_64(
    const std::vector<std::uint64_t>& first,
    const std::vector<std::uint64_t>& second
);
```

$$
c_k=\sum_{i+j=k}\mathtt{first}[i]\,\mathtt{second}[j]\pmod{2^{64}}
$$

を返す。約 30 ビットの 5 個の NTT 素数（$998244353, 1004535809, 985661441, 943718401, 935329793$）で畳み込み、整数としての係数を CRT（mixed radix 表現）で一意に復元してから、下位 64 ビットだけを残す。各入力長が上限以下なら、5 つの法の積は整数係数の最大値より大きい。

## 計算量

| API | 時間計算量 | 空間計算量（戻り値を含む） |
| --- | --- | --- |
| `convolution_mod_2_64(first, second)` | $O(Z\log Z)$ | $O(Z)$ |

$Z$ は `first.size() + second.size() - 1` 以上の最小の 2 の冪である。定数倍として 5 組の NTT を行う。

## 注意点

- いずれかの入力が空なら空列を返す。
- 入力長が上限を超えると `std::length_error` を送出する。
- 復元時の加減乗算のラップアラウンドには、`std::uint64_t` の規格で定められた剰余演算を使う。
- 5 法の CRT は上限までの整数係数を正確に復元するためのもので、3 法の CRT による任意 mod 畳み込み（`arbitrary_mod_convolution`）とは異なる。
