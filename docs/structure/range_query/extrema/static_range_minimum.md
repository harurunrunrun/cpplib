---
title: Static Range Minimum (静的区間最小値) [RMQSQ] [RPLN]
documentation_of: ../../../../src/structure/range_query/extrema/static_range_minimum.hpp
---

静的な列の半開区間の最小値を返す。内部では `SparseTable` を使う。

## StaticRangeMinimum

```cpp
template<class T, int MAX_SIZE = -1>
class StaticRangeMinimum {
public:
    explicit StaticRangeMinimum(const vector<T>& values);
    int size() const;
    T minimum(int left, int right) const;
};
```

`MAX_SIZE` は列の長さの上限である。省略時（`-1`）は上限を設けず、実行時の長さに合わせて `std::vector` で領域を確保する。

## コンストラクタ

静的な列からクエリ用の構造を構築する。

## size

列の長さを返す。

## minimum

半開区間 `[left, right)` の最小値を返す。

## API 別の時間計算量・空間計算量

列の長さを $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N\log N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `minimum(l, r)` | $O(1)$ | $O(1)$ |

`MAX_SIZE` を指定した場合、構築と保存領域は $O(\texttt{MAX_SIZE}\log\texttt{MAX_SIZE})$ になる。

## 注意点

- 列の長さは `MAX_SIZE` 以下でなければならない。
- クエリ区間は `0 <= left <= right <= size()` を満たす必要がある。空区間では `MinMonoid<T>` の単位元を返す。
- 前提に違反すると `std::runtime_error` を送出する。
