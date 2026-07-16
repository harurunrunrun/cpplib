---
title: Range Affine Point Get (区間affine作用一点取得) [range_affine_point_get]
documentation_of: ../src/structure/segtree/range_affine_point_get.hpp
---

値列への区間affine作用と一点取得を行う。

## RangeAffinePointGet

```cpp
template<class T, int MAX_SIZE>
class RangeAffinePointGet {
public:
    using affine_type = MonoidAffine<T>;

    explicit RangeAffinePointGet(const vector<T>& values);

    int size() const;
    void apply(int left, int right, affine_type function);
    void apply(int left, int right, T multiplier, T addend);
    T get(int index);
};
```

## affine_type

`multiplier*x+addend` を表す `MonoidAffine<T>` のalias。

## Constructor

`values` を初期値として構築する。固定容量treeはheap上に確保する。

## size

値列の長さを返す。

## apply

`[left,right)` の各値 `x` を
`function.multiplier*x+function.addend` に変更する。4引数版も同じ作用を表す。

## get

`index` 番目へ保留中の作用を反映し、その値を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(\mathtt{MAX\_SIZE})$ | $O(\mathtt{MAX\_SIZE})$ |
| `size()` | $O(1)$ | $O(1)$ |
| `apply(...)` | $O(\log \mathtt{MAX\_SIZE})$ | $O(1)$ |
| `get(...)` | $O(\log \mathtt{MAX\_SIZE})$ | $O(1)$ |

## 注意点

- `values.size() <= MAX_SIZE` が必要である。
- 点は `[0,size())`、区間は `0 <= left <= right <= size()` でなければならない。
  違反時は `runtime_error` を送出する。
- 後から指定した作用は、それ以前の作用の後に適用される。
- `T` は `0`、`1` と区間長から構築でき、加算と乗算に対応する必要がある。
- objectのcopyはできない。
