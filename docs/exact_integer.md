---
title: Exact Integer (任意精度整数)
documentation_of: ../src/algorithm/math/integer/exact_integer.hpp
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
| ExactInteger同士の `+`, `-`, `+=`, `-=` | $O(\max(L,R))$ | $O(\max(L,R))$ |
| ExactInteger同士の `*`, `*=` | NTT範囲では $O(P\log P)$、それ以外の長さが同程度なら $O(P^{\log_3 5})$、最悪 $O(P^{\log_2 3})$ | $O(P)$ |
| 組み込み整数を右辺に取る `+=`, `-=`, `*=` | $O(L)$ | $O(L)$ |

短い方が32 limb以下なら学校式を使い、それより長い場合は
以下の条件でNTT・Toom-Cook-3・Karatsuba法を切り替える。
短い方が512 limb以上かつ共通の $2^{21}$ 乗根で扱える長さなら、15bit digitへ
変換して2個のNTT素数で畳み込み、CRTで整数係数を一意に復元する。この経路は
$O(P\log P)$ 時間、$O(P)$ 追加領域であり、浮動小数点誤差を持たない。
NTT範囲外では、短い方が192 limb以上で、両方を3個の非空chunkへ分けられる程度に
長さが近い場合にToom-Cook-3法へ切り替える。この場合は5回の再帰乗算で済むため
$O(P^{\log_3 5})$ である。各閾値は、変換・評価・補間・一時領域確保の
線形overheadを小入力で負担しないために設けている。
長さが偏った入力を含む最悪上界は、短い方を0で補ったKaratsuba法の
$O(P^{\log_2 3})$ である。

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
$D$ を10進桁数、$M_{10}(n)$ を $n$ 個の10進chunkの乗算時間とすると、
時間計算量は $O(M_{10}(L)\log L + D)$、追加領域は
$O(L\log L+D)$ である。10進変換用の内部乗算はKaratsuba法なので
$M_{10}(n)=O(n^{\log_2 3})$ である。

2進limbを1個ずつ10進へ変換し、隣接ブロックを
$2^{32w}$（$w$ は下位ブロック幅）倍して平衡木状に統合する。
したがって、値全体を小さい10進基数で繰り返し除算しない。

## 注意点

一般の任意精度整数同士の除算・剰余は提供しない。
乗算は学校式・Karatsuba法・Toom-Cook-3法・2法NTTをサイズと長さ比で切り替える。
NTTは長さ上限内だけで選択し、それを超える入力は正確な再帰乗算へ戻る。
Toom-Cook-3の評価と補間は符号付き整数上で行い、2と3による完全除算を使うため、
浮動小数点の丸めや外部多倍長整数には依存しない。
メモリ確保に失敗した場合は標準コンテナ由来の例外を送出する。
