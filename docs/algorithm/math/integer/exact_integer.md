---
title: Exact Integer (任意精度整数)
documentation_of: ../../../../src/algorithm/math/integer/exact_integer.hpp
---

符号と 2 進の limb（32 bit の桁）列で整数を正確に保持する、外部ライブラリに依存しない任意精度整数である。整数幾何やオーバーフローしうる比較式など、除算を主目的としない正確な中間計算に使う。

以下で $L$ と $R$ は各オペランドの 32 bit limb の個数、$P=\max(L,R)$、$K$ はシフト量とする。

## `ExactInteger`

```cpp
ExactInteger();
template<class Integer>
ExactInteger(Integer value);
```

0、または 128 bit 以下の組み込み整数から構築する。0 の符号は常に非負に正規化される。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| デフォルト構築 | $O(1)$ | $O(1)$ |
| 整数からの構築・代入 | $O(1)$ | $O(1)$ |

## 符号・大きさ

```cpp
bool is_zero() const;
bool is_negative() const;
size_t bit_length() const;
ExactInteger absolute() const;
ExactInteger abs(const ExactInteger& value);
```

`bit_length()` は絶対値を 2 進表現したときの bit 数を返し、0 に対しては 0 を返す。`absolute()` と `abs(value)` は絶対値を返す。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `is_zero`, `is_negative` | $O(1)$ | $O(1)$ |
| `bit_length` | $O(1)$ | $O(1)$ |
| `absolute`, `abs` | $O(L)$ | $O(L)$ |

## 比較

```cpp
bool operator==(const ExactInteger&, const ExactInteger&);
strong_ordering operator<=>(const ExactInteger&, const ExactInteger&);
```

符号付き整数として比較する。時間計算量は $O(\max(L,R))$、追加領域は $O(1)$ である。

## 加減算・乗算

```cpp
ExactInteger operator-() const;
ExactInteger operator+(ExactInteger, const ExactInteger&);
ExactInteger operator-(ExactInteger, const ExactInteger&);
ExactInteger operator*(ExactInteger, const ExactInteger&);
ExactInteger& operator+=(const ExactInteger&);
ExactInteger& operator-=(const ExactInteger&);
ExactInteger& operator*=(const ExactInteger&);

template<class Integer>
ExactInteger& operator+=(Integer value);
template<class Integer>
ExactInteger& operator-=(Integer value);
template<class Integer>
ExactInteger& operator*=(Integer value);
```

小さい組み込み整数を複合代入演算子の右辺に直接渡せる。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 単項 `-` | $O(L)$ | $O(L)$ |
| `ExactInteger` 同士の `+`, `-`, `+=`, `-=` | $O(\max(L,R))$ | $O(\max(L,R))$ |
| `ExactInteger` 同士の `*`, `*=` | Goldilocks NTT の対応範囲では $O(P\log P)$、範囲外で長さが同程度なら $O(P^{\log_3 5})$、最悪 $O(P^{\log_2 3})$ | $O(P)$ |
| 組み込み整数を右辺に取る `+=`, `-=`, `*=` | $O(L)$ | $O(L)$ |

短い方が 32 limb 以下なら筆算（学校式）を使い、それより長い場合は NTT・Toom-Cook-3 法・Karatsuba 法を入力長に応じて切り替える。

短い方が 512 limb 以上で、15 bit の digit 列の畳み込み長が $2^{32}$ 以下なら、$p=2^{64}-2^{32}+1$ を法とする Goldilocks NTT を使う。この素数は 1 の原始 $2^{32}$ 乗根を持つ。短い方の digit 列は最大 $2^{31}$ 要素なので、各係数は $2^{31}(2^{15}-1)^2<p$ を満たす。したがって 1 回の変換だけで整数係数を一意に復元でき、CRT や浮動小数点の丸めを必要としない。この経路は $O(P\log P)$ 時間、$O(P)$ 追加領域である。

NTT の長さ上限を超える場合は、短い方が 192 limb 以上で、両方を 3 個の空でないチャンクに分けられる程度に長さが近いときに Toom-Cook-3 法へ切り替える。この場合は 5 回の再帰乗算で済むため $O(P^{\log_3 5})$ である。各閾値は、変換・評価・補間・一時領域確保の線形のオーバーヘッドを小さい入力で負担しないために設けている。長さが偏った入力を含む最悪の上界は、短い方を 0 で補った Karatsuba 法の $O(P^{\log_2 3})$ である。

## ビットシフト

```cpp
ExactInteger operator<<(ExactInteger value, size_t shift);
ExactInteger operator>>(ExactInteger value, size_t shift);
ExactInteger& operator<<=(size_t shift);
ExactInteger& operator>>=(size_t shift);
```

左シフトは $2^{\mathrm{shift}}$ 倍する。右シフトは $2^{\mathrm{shift}}$ で割った値を負の無限大方向へ丸めるため、負数についても C++20 の符号付き右シフトと同じ意味になる。

左シフトの時間・空間計算量は $O(L+K/32)$、右シフトは $O(L)$ である。結果の格納長がコンテナの上限を超える左シフトは `length_error` を送出する。

## 小さい正整数による `divmod`

```cpp
pair<ExactInteger, uint64_t> divmod(uint64_t positive_divisor) const;
```

正の 64 bit 整数で絶対値を割る。返す商の符号は元の値と同じで、0 方向へ丸める。余り $r$ は `0 <= r < positive_divisor` を満たす。返り値を `[q, r]` とすると `abs(value) == abs(q) * positive_divisor + r` である。

時間計算量は $O(L)$（1 bit ずつ処理するため定数は約 32 倍）、追加領域は $O(L)$ である。`positive_divisor == 0` では `domain_error` を送出する。

## 組み込み整数への変換

```cpp
template<class Integer>
Integer checked_to() const;
```

値が変換先の整数型に収まる場合だけ変換する。範囲外では `overflow_error` を送出する。時間計算量は $O(L)$、追加領域は $O(1)$ である。

## 10 進文字列・出力

```cpp
string to_string() const;
ostream& operator<<(ostream& stream, const ExactInteger& value);
```

符号付き 10 進表記を返す、またはストリームへ出力する。$D$ を 10 進桁数、$M_{10}(n)$ を $n$ 個の 10 進チャンク（$10^9$ 進の桁）の乗算時間とすると、時間計算量は $O(M_{10}(L)\log L + D)$、追加領域は $O(L\log L+D)$ である。10 進チャンクを 3 個の 1000 進の digit に分解し、256 チャンク以上では Goldilocks NTT で内部の乗算を行う。1000 進の係数も $2^{31}\cdot999^2<p$ なので 1 回の変換で正確に復元できる。したがって NTT の対応範囲では $M_{10}(n)=O(n\log n)$ であり、10 進変換全体は $O(L\log^2 L+D)$ となる。小さい入力では筆算または Karatsuba 法に切り替える。

2 進の limb を 1 個ずつ 10 進に変換し、隣接するブロックを $2^{32w}$（$w$ は下位ブロックの幅）倍して平衡木状に統合する。したがって、値全体を小さい 10 進基数で繰り返し除算することはない。

## 注意点

- 一般の任意精度整数同士の除算・剰余は提供しない（`BigInteger` を使う）。
- 乗算は筆算・Karatsuba 法・Toom-Cook-3 法・Goldilocks NTT をサイズと長さの比で切り替える。NTT は長さ上限内だけで選択し、それを超える入力は正確な再帰乗算に戻る。
- Toom-Cook-3 の評価と補間は符号付き整数上で行い、2 と 3 による完全除算を使うため、浮動小数点の丸めや外部の多倍長整数ライブラリには依存しない。
- メモリ確保に失敗した場合は標準コンテナ由来の例外を送出する。
