---
title: Exact Integer (任意精度整数)
documentation_of: ../src/algorithm/math/exact_integer.hpp
---

符号と2進limbで整数を正確に保持する、外部ライブラリに依存しない任意精度整数である。
整数幾何やoverflowし得る比較式など、除算を主目的としない正確な中間計算に使う。

以下で $L$ と $R$ は各オペランドの32bit limb数、
$P=\max(L,R)$、$K$ はshift量とする。

## `ExactInteger`

```cpp
ExactInteger();
template<class Integer>
ExactInteger(Integer value);
```

0、または128bit以下の組み込み整数から構築する。0の符号は常に非負に正規化される。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| default構築 | $O(1)$ | $O(1)$ |
| 整数からの構築・代入 | $O(1)$ | $O(1)$ |

## 符号・大きさ

```cpp
bool is_zero() const;
bool is_negative() const;
size_t bit_length() const;
ExactInteger absolute() const;
ExactInteger abs(const ExactInteger& value);
```

`bit_length()` は絶対値を2進表現したbit数を返し、0では0を返す。
`absolute()` と `abs(value)` は絶対値を返す。

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

小さい組み込み整数をcompound assignmentの右辺へ直接渡せる。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 単項 `-` | $O(L)$ | $O(L)$ |
| `+`, `-`, `+=`, `-=` | $O(\max(L,R))$ | $O(\max(L,R))$ |
| `*`, `*=` | $O(P^{\log_2 3})$ | $O(P\log P)$ |

小さいオペランドには定数倍の小さい学校式を使い、両方が32 limbを超えると
Karatsuba法へ切り替える。上表は短い方を0で補った最悪上界であり、強く長さが
偏った入力では再帰が空の上位部分を省く。

## bit shift

```cpp
ExactInteger operator<<(ExactInteger value, size_t shift);
ExactInteger operator>>(ExactInteger value, size_t shift);
ExactInteger& operator<<=(size_t shift);
ExactInteger& operator>>=(size_t shift);
```

左shiftは $2^{shift}$ 倍する。右shiftは $2^{shift}$ で割った値を負の無限大へ丸めるため、
負数についてもC++20の符号付き右shiftと同じ意味になる。

左shiftの時間・空間計算量は $O(L+K/32)$、右shiftは $O(L)$ である。
結果の格納長がコンテナ上限を超える左shiftは `length_error` を送出する。

## 小さい正整数によるdivmod

```cpp
pair<ExactInteger, uint64_t> divmod(uint64_t positive_divisor) const;
```

正の64bit整数で絶対値を割る。返す商の符号は元の値と同じで0方向へ丸め、
余り $r$ は `0 <= r < positive_divisor` を満たす。
返値を `[q, r]` とすると
`abs(value) == abs(q) * positive_divisor + r` である。

時間計算量は $O(L)$、追加領域は $O(L)$ である。
`positive_divisor == 0` では `domain_error` を送出する。

## 組み込み整数への変換

```cpp
template<class Integer>
Integer checked_to() const;
```

値が変換先の整数型に収まる場合だけ変換する。範囲外では `overflow_error` を送出する。
時間計算量は $O(L)$、追加領域は $O(1)$ である。

## 10進文字列・出力

```cpp
string to_string() const;
ostream& operator<<(ostream& stream, const ExactInteger& value);
```

符号付き10進表記を返す、またはstreamへ出力する。
$D$ を10進桁数、$M(n)$ を $n$ 個のlimbの乗算時間とすると、時間計算量は
$O(M(L)\log L + D)$、追加領域は $O(L\log L+D)$ である。
現在の内部乗算はKaratsuba法なので $M(n)=O(n^{\log_2 3})$ である。

2進limbを1個ずつ10進へ変換し、隣接ブロックを
$2^{32w}$（$w$ は下位ブロック幅）倍して平衡木状に統合する。
したがって、値全体を小さい10進基数で繰り返し除算しない。

## 注意点

一般の任意精度整数同士の除算・剰余は提供しない。
乗算は学校式とKaratsuba法をサイズで切り替える。さらに巨大な整数を大量に
乗算する用途ではFFT系の多項式演算も検討する。
メモリ確保に失敗した場合は標準コンテナ由来の例外を送出する。
