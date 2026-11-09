---
title: Fast Big Integer (高速多倍長整数) [addition_of_big_integers] [addition_of_hex_big_integers] [division_of_big_integers] [division_of_hex_big_integers] [multiplication_of_big_integers] [multiplication_of_hex_big_integers]
documentation_of: ../../../../src/algorithm/math/integer/fast_big_integer.hpp
---

`BasicBigInteger<RADIX>` は 10 進・16 進の入出力と符号付き四則演算を提供する。別名 `BigInteger = BasicBigInteger<10>` と `HexBigInteger = BasicBigInteger<16>` が使える。内部表現は符号と little-endian の `uint32_t` limb 列で、10 進版は $10^9$ 進、16 進版は $2^{28}$ 進の limb を使い、16 進 7 桁を 1 limb として直接入出力する。16 進入力は大文字・小文字のどちらも許し、出力は大文字に統一する。基数変換を行わないため、16 進文字列の入出力も桁数に対して線形時間である。

4 limb まではオブジェクト内に保持する（small buffer）。外部の多倍長整数ライブラリには依存せず、GCC 13、C++20、`__uint128_t` を前提とする。

オブジェクト内の配列とヒープ用の `std::vector` は `union` で領域を共有するため、GCC 13 の 64 bit 環境では `sizeof(BigInteger) == 32` である。多数の値を優先度付きキューや配列に保持する用途でも、未使用のヒープ管理領域を各オブジェクトが重複して持たない。

以下、limb 数を $n$、乗算の計算量を $M(n)$ とする。

## 構築・代入

```cpp
BigInteger()
template<class Integer> BigInteger(Integer value)
explicit BigInteger(std::string_view decimal)
template<class Integer> BigInteger& operator=(Integer value)
BigInteger& assign(std::string_view decimal)
```

組み込み整数または符号付き 10 進文字列（`HexBigInteger` では 16 進文字列）から構築する。文字列では先頭の `+` / `-` と先頭の 0 を許す。空文字列、符号だけの文字列、数字以外を含む文字列では `std::invalid_argument` を送出する。入力は下位から 9 桁（16 進版は 7 桁）ずつ直接 limb に変換するため、桁数に対して線形時間である。

0 は常に非負に正規化される。

## 状態取得・変換

```cpp
bool is_zero() const noexcept
bool is_negative() const noexcept
BigInteger absolute() const
std::string to_string() const
template<class Integer> Integer checked_to() const
```

`to_string` は最上位の limb を通常の幅、それ以外を 9 桁（16 進版は 7 桁）の固定幅で直接連結し、桁数に対して線形時間で動作する。`checked_to<Integer>()` は、値が変換先の組み込み整数型に収まらない場合に `std::overflow_error` を送出する。

非メンバ関数 `abs(value)` も利用できる。

## 四則演算

```cpp
BigInteger& operator+=(const BigInteger&)
BigInteger& operator-=(const BigInteger&)
BigInteger& operator*=(const BigInteger&)
BigInteger& operator/=(const BigInteger&)
BigInteger& operator%=(const BigInteger&)

BigInteger operator+(const BigInteger&, const BigInteger&)
BigInteger operator-(const BigInteger&, const BigInteger&)
BigInteger operator*(const BigInteger&, const BigInteger&)
BigInteger operator/(const BigInteger&, const BigInteger&)
BigInteger operator%(const BigInteger&, const BigInteger&)
```

加減算は limb 数に対して線形である。乗算は短い側が 128 limb 以下なら筆算（schoolbook 法）、それより長ければ Montgomery 乗算による radix-4 NTT と CRT を使う。複合代入の加減算と、1 limb の値による複合代入の乗除算は、既存の格納領域を再利用する。1 limb の値による除算と剰余は除数ごとに逆数を一度だけ求め、各 limb を 128 bit 積の上位（mul-high）で処理するため、limb ごとにハードウェア除算を繰り返さない。

長い乗算では次の 3 素数を使う。

- `167772161`
- `469762049`
- `754974721`

CRT で各係数を `__uint128_t` に復元してから、limb の基数での繰り上がりを処理する。同じオブジェクト同士の積（二乗）では順変換を 1 回に省略する。必要な NTT 長が $2^{24}$ を超える場合は `std::length_error` を送出する。

## 除算と `divmod`

```cpp
static std::pair<BigInteger, BigInteger> divmod(
    const BigInteger& dividend,
    const BigInteger& divisor
)
```

除数が 1 limb の場合の除算、除数が 64 limb 以下の場合の正規化した筆算（long division）、それより大きい場合の Newton 法による逆数計算を切り替える。`divmod` は商と余りを同時に求めるので、両方が必要な場合は `/` と `%` を個別に呼ぶより効率がよい。

`/` と `%` を単独で使う場合も、筆算では不要な側の結果の `std::vector` を確保しない。特に `value % small_integer` は左辺のコピーや商の構築をせず、剰余だけを計算する。

除数が 0 なら `std::domain_error` を送出する。商は 0 方向へ丸め、余りは被除数と同符号である。返される商 `q` と余り `r` は常に次を満たす。

```text
dividend == q * divisor + r
abs(r) < abs(divisor)
```

大きさが釣り合った大きな入力の除算は $O(M(n) \log n)$ 時間である。

## 比較・符号・入出力

```cpp
BigInteger operator-() const
BigInteger& operator++()
BigInteger& operator--()
bool operator==(const BigInteger&, const BigInteger&)
std::strong_ordering operator<=>(const BigInteger&, const BigInteger&)
std::ostream& operator<<(std::ostream&, const BigInteger&)
std::istream& operator>>(std::istream&, BigInteger&)
```

後置の `++` / `--` も提供する。ストリーム入力のトークンが不正な文字列なら `failbit` を設定し、代入先の値は変更しない。

## 時間計算量

- 入出力・加減算: limb 数に対して線形
- 乗算: 筆算は $O(n^2)$、NTT 乗算は $O(n \log n)$
- 除算: Newton 法では $O(M(n) \log n)$
- 追加空間: 大きな演算で $O(n)$

## 注意点

- この高速版は Big Integer 系の問題向けの API である。`ExactInteger`、`exact_integer()`、`bit_length()`、ビットシフト、式テンプレート型は提供しない。
- 大域名前空間に `BigInteger` を定義するため、同名の型を定義する `big_integer.hpp` と同時には include できない。

## 類題

符号付き 16 進整数の積を、別の 16 進整数で割った商と剰余を求める問題は、`HexBigInteger::divmod(a * b, c)` で処理できる。丸めは 0 方向、剰余の符号は被除数に従う。
