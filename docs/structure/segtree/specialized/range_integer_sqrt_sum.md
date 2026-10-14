---
title: Range Integer Square Root Sum (区間整数平方根・区間和) [GSS4]
documentation_of: ../../../../src/structure/segtree/specialized/range_integer_sqrt_sum.hpp
---

区間内の各値を整数平方根へ置換し、区間和を取得する列です。

## API

### `RangeIntegerSqrtSum(values)`

初期列から構築します。

### `size()`

要素数を返します。

### `apply_sqrt(left, right)`

半開区間 `[left, right)` の各値 $x$ を $\lfloor\sqrt{x}\rfloor$ に置換します。

### `sum(left, right)`

半開区間 `[left, right)` の和を返します。

## API別の時間計算量・空間計算量

`apply_sqrt` で実際に調べる不安定要素数を $K$ とします。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N\log N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `apply_sqrt(l,r)` | $O((K+1)\log N)$ | $O(1)$ |
| `sum(l,r)` | $O(\log N)$ | $O(1)$ |

各非負要素が不安定要素として更新される回数は、初期値を $A$ として $O(\log\log(A+2))$ 回です。

## 注意点

- `N <= MAX_SIZE` が必要です。
- 想定する値は非負です。値が $0$ または $1$ になると以降の平方根更新を省略します。
- 区間は半開区間で、範囲違反では基盤構造が `runtime_error` を送出します。

## 容量を省略する場合

容量引数の既定値は `-1`。`RangeIntegerSqrtSum<> tree(values);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
