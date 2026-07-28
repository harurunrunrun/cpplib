---
title: Convolution over GF(2^64) (64次拡大二元体上の畳み込み) [convolution_F_2_64]
documentation_of: ../../../../src/algorithm/fft/transform/convolution_gf2_64.hpp
---

$$
\mathbb F_{2^{64}}
=\mathbb F_2[x]/(x^{64}+x^4+x^3+x+1)
$$

を、係数をbitとみなした `uint64_t` で表す。

## 体要素

```cpp
class Gf2_64;
```

- `Gf2_64(uint64_t value)`: 64 bit多項式から構築する。
- `uint64_t value() const`: 同じ表現を返す。
- `operator+`, `operator-`: 係数ごとのXOR。
- `operator*`: carry-less積を既約多項式で剰余化した積。
- `pow(exponent)`: 非負整数冪。
- `inverse()`: 乗法逆元。0では `std::domain_error`。

x86ではPCLMUL対応を実行時に調べ、対応CPUだけcarry-less命令を使う。
それ以外ではportableなbit演算へ切り替える。

## 対応上限

```cpp
inline constexpr size_t gf2_64_convolution_max_input_size;
```

各入力列について保証する最大長 `524288` を表す。

## 畳み込み

```cpp
vector<uint64_t> gf2_64_convolution(
    const vector<uint64_t>& first,
    const vector<uint64_t>& second
);
```

$$
c_k=\sum_{i+j=k}\mathtt{first}[i]\mathtt{second}[j]
\quad\text{in }\mathbb F_{2^{64}}
$$

を返す。二次拡大環上の3進Schönhage再帰を用い、根の位数を
入力長に合わせて再帰的に増やす。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（戻り値を含む） |
| --- | --- | --- |
| `Gf2_64::{+,-,*}` | $O(1)$ | $O(1)$ |
| `Gf2_64::pow(exponent)` | $O(\log exponent)$ | $O(1)$ |
| `Gf2_64::inverse()` | $O(64)$ | $O(1)$ |
| `gf2_64_convolution_max_input_size` | $O(1)$ | $O(1)$ |
| `gf2_64_convolution(first, second)` | $O(Z\log Z\log\log Z)$ | $O(Z)$ |

$Z$ は出力長以上となる再帰変換長である。短い積は二重ループへ切り替える。

## 注意点

- いずれかの入力が空なら空列を返す。
- 上限を超える入力では `std::length_error` を送出する。
- 整数の加算・乗算や法 $2^{64}$ の演算ではない。
- GCC 13のx86 target属性はPCLMUL関数だけに付与し、
  translation unit全体の命令セットは変更しない。
