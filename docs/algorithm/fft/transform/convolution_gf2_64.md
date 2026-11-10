---
title: Convolution over GF(2^64) (64次拡大二元体上の畳み込み) [convolution_F_2_64]
documentation_of: ../../../../src/algorithm/fft/transform/convolution_gf2_64.hpp
---

$$
\mathbb F_{2^{64}}
=\mathbb F_2[x]/(x^{64}+x^4+x^3+x+1)
$$

の元を、多項式の係数を各ビットとみなした `std::uint64_t` で表し、その上での畳み込みを計算する。

## 体の元

```cpp
class Gf2_64;
```

- `Gf2_64()`: 零元を作る。
- `explicit Gf2_64(std::uint64_t value)`: 64 ビットで表した多項式から構築する。
- `std::uint64_t value() const`: 同じ表現を返す。
- `operator+`, `operator-`, 単項 `operator-`: 係数ごとの XOR（単項マイナスは自分自身）。
- `operator*`: 繰り上がりなし乗算（carry-less 乗算）の結果を既約多項式で割った余り。
- `operator==`: 等値比較。
- `pow(exponent)`: 非負整数（`std::uint64_t`）乗。
- `inverse()`: 乗法逆元を $a^{2^{64}-2}$ として求める。$0$ に対しては `std::domain_error` を送出する。

x86 では PCLMUL 命令への対応を実行時に調べ、対応する CPU でのみ carry-less 乗算命令を使う。それ以外ではビット演算による移植性のある実装に切り替える。

## 対応上限

```cpp
inline constexpr std::size_t gf2_64_convolution_max_input_size = 524288;
```

各入力列について動作を保証する最大長である。

## 畳み込み

```cpp
std::vector<std::uint64_t> gf2_64_convolution(
    const std::vector<std::uint64_t>& first,
    const std::vector<std::uint64_t>& second
);
```

$$
c_k=\sum_{i+j=k}\mathtt{first}[i]\,\mathtt{second}[j]
\quad\text{in }\mathbb F_{2^{64}}
$$

を返す。二次拡大環上の 3 進 Schönhage 法の再帰を用い、1 の冪根の位数を入力長に合わせて再帰的に増やす。

## 計算量

| API | 時間計算量 | 空間計算量（戻り値を含む） |
| --- | --- | --- |
| `Gf2_64::{+,-,*}` | $O(1)$ | $O(1)$ |
| `Gf2_64::pow(exponent)` | $O(\log \texttt{exponent})$ | $O(1)$ |
| `Gf2_64::inverse()` | $O(64)$ | $O(1)$ |
| `gf2_64_convolution(first, second)` | $O(Z\log Z\log\log Z)$ | $O(Z)$ |

$Z$ は再帰変換の長さで、$2Z$ が出力長以上となる最小の $3$ の冪である。積が短い場合は二重ループに切り替える。

## 注意点

- いずれかの入力が空なら空列を返す。
- 入力長が上限を超えると `std::length_error` を送出する。
- 整数の加算・乗算や法 $2^{64}$ での演算ではない。
- GCC 13 の x86 向け target 属性は PCLMUL を使う関数だけに付け、翻訳単位全体の命令セットは変更しない。
