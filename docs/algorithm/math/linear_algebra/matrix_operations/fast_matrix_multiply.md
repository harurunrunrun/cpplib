---
title: Fast Matrix Multiplication (高速行列積)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/matrix_operations/fast_matrix_multiply.hpp
---

Strassen 法による、正確な体・環上の密行列積。

通常の行列積とは別の API であり、小さい部分行列では単純な三重ループに切り替える。細長い長方形行列では、実際の $rkc$ が十分小さければ長方形のまま単純な積を計算する。それ以外の長方形行列や辺の長さが 2 の冪でない行列は、各辺を 0 で埋めて同じ大きさの正方行列にしてから計算する。

```cpp
using Mint = Modint998244353;
std::vector<std::vector<Mint>> values{ {1, 2}, {3, 4} };
Matrix<Mint> left(values), right(values);
auto product = fast_matrix_multiply(left, right);
```

## 行優先配列の API

```cpp
vector<T> fast_matrix_multiply(
    const vector<T>& left,
    size_t left_rows,
    size_t inner_size,
    const vector<T>& right,
    size_t right_columns
)
```

`left` を `left_rows` 行 `inner_size` 列、`right` を `inner_size` 行 `right_columns` 列の行優先（row-major）配列と解釈し、積を行優先配列で返す。この API では 0 行または 0 列を含むすべての形の行列を表現できる。

入力配列は変更しない。同じ配列を `left` と `right` の両方に渡してもよい。

## `Matrix` の API

```cpp
fast_matrix_multiply(left, right)
```

`Matrix` を受け取り、`Matrix<T, LEFT_MAX_ROW, RIGHT_MAX_COL>` を返すオーバーロード。実行時の列数と行数が一致していれば、左右のコンパイル時の最大サイズは異なっていてもよい。容量を省略した可変長の `Matrix<T>` を渡すと、返り値も可変長版になる。

## 時間計算量

$r$ を `left_rows`、$k$ を `inner_size`、$c$ を `right_columns`、$S=\max(r,k,c)$、$\omega=\log_2 7$ とする。固定長の `Matrix` では、返り値の容量 `LEFT_MAX_ROW`, `RIGHT_MAX_COL` を $R_{\max}$, $C_{\max}$ と書く。

| API | 時間計算量 |
| --- | --- |
| 行優先配列の API、可変長 `Matrix` | $O(S^\omega)$ |
| 固定長 `Matrix` | $O(S^\omega + R_{\max} C_{\max})$ |

行優先配列の API は $S$ 以上で最小の 2 の冪 $P$ を求める。固定の閾値を $D$ とし、$P\le D$ のとき $B(P)=P^3$、$P>D$ のとき $B(P)=7B(P/2)$ と定義する。$rkc\le B(P)$ なら長方形のまま単純な積を、そうでなければ $P\times P$ に 0 埋めした Strassen 法を選ぶ。$B(P)=O(P^\omega)$ なので、前者も $O(S^\omega)$ である。$S>0$ なら $P<2S$ なので、後者の再帰も $O(P^\omega)=O(S^\omega)$ である。0 埋め、入力のコピー、結果の切り出しは $O(P^2)$ であり、上記の計算量に含まれる。

いずれかの次元が 0 の場合は $O(rc)$ で零行列を返し、これも $O(S^\omega)$ の上界に含まれる。固定長 `Matrix` の追加の項は、固定長の戻り値の保存領域を値初期化するという `Matrix` の仕様による。

`T` の構築・コピー・加減乗算を $O(1)$ とした計算量である。

## 空間計算量

| API | 追加領域・戻り値 |
| --- | --- |
| 行優先配列の API、可変長 `Matrix` | $O(S^2)$ |
| 固定長 `Matrix` | $O(S^2 + R_{\max} C_{\max})$ |

再帰中の一時領域は 1 つの作業領域を深さ方向に再利用するため、再帰木の全節点分を同時には確保しない。

## 注意点

- `T{}` が加法単位元であり、`T` に加算・減算・乗算・代入が定義されている必要がある。
- 減算を使うため、減算を持たない一般の半環には使えない。除算は使わない。
- 整数型では、通常の演算と Strassen 法の中間値が型の範囲に収まる必要がある。
- 浮動小数点型でもコンパイルできるが、演算順が通常の積と異なるため丸め誤差が増えうる。正確な体・環を主な対象とする。
- 行優先配列の API は `left.size() == r*k` かつ `right.size() == k*c` を要求する。
- `Matrix` の API は `left.cols() == right.rows()` を要求する。
- 形の不一致では `invalid_argument`、サイズ計算または 0 埋めのオーバーフローでは `length_error` を送出する。
