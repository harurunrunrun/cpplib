---
title: Range Affine Range Sum Large Array (巨大配列区間affine作用区間和) [range_affine_range_sum_large_array]
documentation_of: ../../../../src/structure/segtree/specialized/range_affine_range_sum_large_array.hpp
---

巨大な零初期化配列を疎に保持し、区間affine作用と区間和取得を行う。

## RangeAffineRangeSumLargeArray

```cpp
template<class T, long long MAX_SIZE>
class RangeAffineRangeSumLargeArray {
public:
    using affine_type = MonoidAffine<T>;

    explicit RangeAffineRangeSumLargeArray(long long size);

    long long size() const;
    void apply(long long left, long long right, affine_type function);
    void apply(
        long long left, long long right, T multiplier, T addend
    );
    T range_sum(long long left, long long right) const;
};
```

## affine_type

`multiplier*x+addend` を表す `MonoidAffine<T>` のalias。

## Constructor

長さ `size` の零列を構築する。nodeは必要になった区間にだけ生成する。

## size

値列の長さを返す。

## apply

`[left,right)` の各値 `x` を
`function.multiplier*x+function.addend` に変更する。4引数版も同じ作用を表す。

## range_sum

`[left,right)` の値の和を返す。空区間では `T(0)` を返す。

## API別の時間計算量・空間計算量

生成済みnode数を $K$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(1)$ | $O(1)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `apply(...)` | $O(\log \mathtt{MAX\_SIZE})$ | $O(\log \mathtt{MAX\_SIZE})$ node/stack |
| `range_sum(...)` | $O(\log \mathtt{MAX\_SIZE})$ | $O(\log \mathtt{MAX\_SIZE})$ stack |

object全体の保存領域は $O(K)$。

## 注意点

- $0<\mathtt{MAX\_SIZE}<2^{62}$ かつ
  $0\leq size\leq\mathtt{MAX\_SIZE}$ が必要である。
- 区間は `0 <= left <= right <= size()` でなければならない。
  違反時は `runtime_error` を送出する。
- `T` は `0`、`1` と区間長から構築でき、加算と乗算に対応する必要がある。
- objectのcopyはできない。node確保に失敗した場合は `bad_alloc` を送出する。

## 容量を省略する場合

容量引数の既定値は `-1`。`RangeAffineRangeSumLargeArray<T> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
