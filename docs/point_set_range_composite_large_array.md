---
title: Point Set Range Composite Large Array (巨大配列一点変更区間合成) [point_set_range_composite_large_array]
documentation_of: ../src/structure/segtree/point_set_range_composite_large_array.hpp
---

巨大な添字範囲のaffine関数列を疎に保持し、一点変更と区間合成値の評価を行う。

# PointSetRangeCompositeLargeArray

```cpp
template<class T, long long MAX_SIZE>
class PointSetRangeCompositeLargeArray {
public:
    using affine_type = MonoidAffine<T>;

    explicit PointSetRangeCompositeLargeArray(long long size);

    long long size() const;
    void set(long long index, affine_type function);
    void set(long long index, T multiplier, T addend);
    T evaluate(long long left, long long right, T value) const;
};
```

## affine_type

`multiplier*x+addend` を表す `MonoidAffine<T>` のalias。

## Constructor

長さ `size` の恒等関数列を構築する。nodeは必要になった添字にだけ生成する。

## size

関数列の長さを返す。

## set

`index` 番目の関数を置換する。3引数版は
`affine_type{multiplier, addend}` を設定する。

## evaluate

`[left,right)` の関数を添字の昇順で `value` へ適用した結果を返す。
未設定点は恒等関数であり、空区間では `value` を返す。

## API別の時間計算量・空間計算量

生成済みnode数を $K$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(1)$ | $O(1)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `set(...)` | $O(\log \mathtt{MAX\_SIZE})$ | $O(\log \mathtt{MAX\_SIZE})$ node |
| `evaluate(...)` | $O(\log \mathtt{MAX\_SIZE})$ | $O(\log \mathtt{MAX\_SIZE})$ stack |

object全体の保存領域は $O(K)$。

## 注意点

- $0<\mathtt{MAX\_SIZE}<2^{62}$ かつ
  $0\leq size\leq\mathtt{MAX\_SIZE}$ が必要である。
- 点は `[0,size())`、区間は `0 <= left <= right <= size()` でなければならない。
  違反時は `runtime_error` を送出する。
- 合成順序は左から右であり、可換性を仮定しない。
- `T` は `0`、`1` から構築でき、加算と乗算に対応する必要がある。
- objectのcopyはできない。node確保に失敗した場合は `bad_alloc` を送出する。
