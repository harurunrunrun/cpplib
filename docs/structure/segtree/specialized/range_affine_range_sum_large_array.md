---
title: Range Affine Range Sum Large Array (巨大配列区間affine作用区間和) [range_affine_range_sum_large_array]
documentation_of: ../../../../src/structure/segtree/specialized/range_affine_range_sum_large_array.hpp
---

0 で初期化された巨大な配列を疎に保持し、区間アフィン作用と区間和の取得を行う。

## RangeAffineRangeSumLargeArray

```cpp
template<class T, long long MAX_SIZE = -1>
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

内部では `DynamicLazySegtree` を使う。`MAX_SIZE` を省略すると可変長版になり、長さはコンストラクタの引数で決まる。

## affine_type

$f(x) = \mathrm{multiplier} \cdot x + \mathrm{addend}$ を表す `MonoidAffine<T>` の別名。

## コンストラクタ

長さ `size` の全 0 列を構築する。ノードは必要になった区間に対してだけ生成する。

## size

値の列の長さを返す。

## apply

半開区間 $[left, right)$ の各値 $x$ を `function.multiplier * x + function.addend` に変更する。4 引数版も同じ作用を表す。

## range_sum

半開区間 $[left, right)$ の値の和を返す。空区間では `T(0)` を返す。

## API 別の時間計算量・空間計算量

生成済みのノード数を $K$ とする。可変長版では $\texttt{MAX_SIZE}$ を列の長さに読み替える。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(1)$ | $O(1)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `apply(...)` | $O(\log \texttt{MAX_SIZE})$ | ノード・スタック $O(\log \texttt{MAX_SIZE})$ |
| `range_sum(...)` | $O(\log \texttt{MAX_SIZE})$ | スタック $O(\log \texttt{MAX_SIZE})$ |

オブジェクト全体の保存領域は $O(K)$ である。

## 注意点

- 固定容量版では $0 \lt \texttt{MAX_SIZE} \lt 2^{62}$ かつ $0 \le \mathrm{size} \le \texttt{MAX_SIZE}$ が必要である。
- 区間は `0 <= left <= right <= size()` でなければならない。違反時は `std::runtime_error` を送出する。
- `T` は `0`, `1` と区間長から構築でき、加算と乗算に対応する必要がある。
- オブジェクトはコピーできない。ノードの確保に失敗した場合は `std::bad_alloc` を送出する。
