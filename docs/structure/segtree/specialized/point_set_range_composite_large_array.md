---
title: Point Set Range Composite Large Array (巨大配列一点変更区間合成) [point_set_range_composite_large_array]
documentation_of: ../../../../src/structure/segtree/specialized/point_set_range_composite_large_array.hpp
---

巨大な添字範囲のアフィン関数の列を疎に保持し、1 点の変更と区間の合成関数の評価を行う。

## PointSetRangeCompositeLargeArray

```cpp
template<class T, long long MAX_SIZE = -1>
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

内部では `DynamicSegtree` を使う。`MAX_SIZE` を省略すると可変長版になり、長さはコンストラクタの引数で決まる。

## affine_type

$f(x) = \mathrm{multiplier} \cdot x + \mathrm{addend}$ を表す `MonoidAffine<T>` の別名。

## コンストラクタ

長さ `size` の恒等関数の列を構築する。ノードは必要になった添字に対してだけ生成する。

## size

関数列の長さを返す。

## set

`index` 番目の関数を置き換える。3 引数版は `affine_type{multiplier, addend}` を設定する。

## evaluate

半開区間 $[left, right)$ の関数を、添字の昇順に `value` へ適用した結果を返す。未設定の点は恒等関数であり、空区間では `value` を返す。

## API 別の時間計算量・空間計算量

生成済みのノード数を $K$ とする。可変長版では $\texttt{MAX_SIZE}$ を列の長さに読み替える。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(1)$ | $O(1)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `set(...)` | $O(\log \texttt{MAX_SIZE})$ | ノード $O(\log \texttt{MAX_SIZE})$ |
| `evaluate(...)` | $O(\log \texttt{MAX_SIZE})$ | スタック $O(\log \texttt{MAX_SIZE})$ |

オブジェクト全体の保存領域は $O(K)$ である。

## 注意点

- 固定容量版では $0 \lt \texttt{MAX_SIZE} \lt 2^{62}$ かつ $0 \le \mathrm{size} \le \texttt{MAX_SIZE}$ が必要である。
- 点は $[0, \mathrm{size})$、区間は `0 <= left <= right <= size()` でなければならない。違反時は `std::runtime_error` を送出する。
- 合成の順序は左から右であり、可換性を仮定しない。
- `T` は `0`, `1` から構築でき、加算と乗算に対応する必要がある。
- オブジェクトはコピーできない。ノードの確保に失敗した場合は `std::bad_alloc` を送出する。
