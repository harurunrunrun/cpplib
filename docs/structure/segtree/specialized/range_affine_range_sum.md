---
title: Range Affine Range Sum (区間affine作用区間和) [range_affine_range_sum]
documentation_of: ../../../../src/structure/segtree/specialized/range_affine_range_sum.hpp
---

値の列に対して、区間アフィン作用と区間和の取得を行う。

## RangeAffineRangeSum

```cpp
template<class T, int MAX_SIZE = -1>
class RangeAffineRangeSum {
public:
    using affine_type = MonoidAffine<T>;
    explicit RangeAffineRangeSum(const vector<T>& values);
    int size() const;
    void apply(int left, int right, affine_type function);
    void apply(int left, int right, T multiplier, T addend);
    T range_sum(int left, int right);
};
```

内部では `LazySegtree` を使う。`MAX_SIZE` を省略すると可変長版になり、長さの上限がなくなる。

## affine_type

$f(x) = \mathrm{multiplier} \cdot x + \mathrm{addend}$ を表す `MonoidAffine<T>` の別名。

## コンストラクタ

`values` を初期値として構築する。固定容量の木はヒープ上に確保する。

## size

値の列の長さを返す。

## apply

半開区間 $[left, right)$ の各値 $x$ を `function.multiplier * x + function.addend` に変更する。4 引数版も同じ作用を表す。

## range_sum

半開区間 $[left, right)$ の値の和を返す。空区間では `T(0)` を返す。遅延作用を押し下げるため `const` ではない。

## API 別の時間計算量・空間計算量

可変長版では $\texttt{MAX_SIZE}$ を列の長さに読み替える。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(\texttt{MAX_SIZE})$ | $O(\texttt{MAX_SIZE})$ |
| `size()` | $O(1)$ | $O(1)$ |
| `apply(...)` | $O(\log \texttt{MAX_SIZE})$ | $O(1)$ |
| `range_sum(...)` | $O(\log \texttt{MAX_SIZE})$ | $O(1)$ |

## 注意点

- 固定容量版では `values.size() <= MAX_SIZE` が必要である。
- 区間は `0 <= left <= right <= size()` でなければならない。違反時は `std::runtime_error` を送出する。
- 後から指定した作用は、それ以前の作用の後に適用される。
- `T` は `0`, `1` と区間長から構築でき、加算と乗算に対応する必要がある。区間和を含むすべての結果は `T` で表現できなければならない。
- オブジェクトはコピーできない。
