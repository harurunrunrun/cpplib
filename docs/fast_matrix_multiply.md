---
title: Fast Matrix Multiplication (高速行列積)
documentation_of: ../src/algorithm/math/fast_matrix_multiply.hpp
---

Strassen 法による、正確な体・環上の密行列積。

通常の行列積とは別 API であり、小さい部分行列は単純な三重ループへ切り替える。
細長い長方形では実際の $rkc$ が十分小さければ長方形のまま単純積を行う。
それ以外の長方形や 2 の冪でない行列は、各辺を 0 で埋めて同じ大きさの
正方行列にしてから計算する。

## Row-major API

```cpp
vector<T> fast_matrix_multiply(
    const vector<T>& left,
    size_t left_rows,
    size_t inner_size,
    const vector<T>& right,
    size_t right_columns
)
```

`left` を `left_rows` 行 `inner_size` 列、`right` を `inner_size` 行
`right_columns` 列の row-major 配列と解釈し、積を row-major 配列で返す。
この API では 0 行または 0 列を含む行列の全ての shape を表現できる。

入力配列は変更しない。同じ配列を `left` と `right` の両方へ渡すこともできる。

## Matrix API

```cpp
fast_matrix_multiply(left, right)
```

既存の固定容量 `Matrix` を受け取り、
`Matrix<T, LEFT_MAX_ROW, RIGHT_MAX_COL>` を返す overload。
実行時の列数と行数が一致していれば、左右の compile-time 最大サイズは異なっていてもよい。

## 時間計算量

$r=left\_rows$, $k=inner\_size$, $c=right\_columns$、
$S=\max(r,k,c)$、$\omega=\log_2 7$ とする。

| API | 時間計算量 |
| --- | --- |
| row-major API | $O(S^\omega)$ |
| `Matrix` API | $O(S^\omega + LEFT\_MAX\_ROW\cdot RIGHT\_MAX\_COL)$ |

row-major API は $S$ 以上で最小の 2 の冪 $P$ を求める。
固定閾値を $D$ とし、$B(P)=P^3\ (P\le D)$、
$B(P)=7B(P/2)\ (P>D)$ と定義する。$rkc\le B(P)$ なら長方形の単純積、
そうでなければ $P\times P$ へ padding した Strassen 法を選ぶ。
$B(P)=O(P^\omega)$ なので、前者も $O(S^\omega)$ である。
$S>0$ なら $P<2S$ なので、後者の再帰も $O(P^\omega)=O(S^\omega)$。
padding、入力コピー、結果の切り出しは $O(P^2)$ であり、上記計算量に含まれる。

いずれかの次元が 0 の場合は $O(rc)$ で 0 行列を返し、これも
$O(S^\omega)$ の上界に含まれる。`Matrix` API の追加項は、固定長の戻り値保存領域を
値初期化する既存 `Matrix` の仕様による。

`T` の構築・コピー・加減乗算を $O(1)$ とした計算量である。

## 空間計算量

| API | 追加領域・戻り値 |
| --- | --- |
| row-major API | $O(S^2)$ |
| `Matrix` API | $O(S^2 + LEFT\_MAX\_ROW\cdot RIGHT\_MAX\_COL)$ |

再帰中の一時領域は一つの workspace を深さ方向に再利用するため、再帰木の全節点分を
同時には確保しない。

## 注意点

- `T{}` が加法単位元であり、`T` に加算・減算・乗算・代入が定義されている必要がある。
- 減算を使うため、減算を持たない一般の半環には利用できない。除算は使わない。
- 整数型では通常の演算と Strassen 法の中間値が型の範囲に収まる必要がある。
- 浮動小数点型にもコンパイルできるが、演算順が通常積と異なるため丸め誤差が増え得る。
  正確な体・環を主対象とする。
- row-major API は `left.size() == r*k` かつ `right.size() == k*c` を要求する。
- `Matrix` API は `left.cols() == right.rows()` を要求する。
- shape 違反では `invalid_argument`、サイズ計算または padding の overflow では
  `length_error` を送出する。
