---
title: Static Range MEX (静的区間MEX)
documentation_of: ../../../../src/structure/range_query/value_query/static_range_mex.hpp
---

静的な整数列の任意の半開区間の MEX を、右端ごとに「各値の最後の出現位置」を持つ永続セグメント木で求める。

## コンストラクタ

```cpp
explicit StaticRangeMex(const vector<int64_t>& values)
```

各接頭辞に対応するバージョンを構築する。

## range_mex

```cpp
size_t range_mex(size_t left, size_t right) const
```

`values[left..right)` の MEX を返す。空区間では 0 を返す。

## size / node_count

```cpp
size_t size() const
size_t node_count() const
```

列の長さと、確保済みのノード数を返す。

## 時間計算量

列の長さを $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N\log N)$ |
| `range_mex` | $O(\log N)$ | $O(\log N)$ の再帰スタック |
| `size`, `node_count` | $O(1)$ | $O(1)$ |

## 注意点

- MEX は $[0, N]$ に入るので、負の値と $N$ より大きい値は木に反映しない。
- `left > right` または `right > size()` の区間では `std::out_of_range` を送出する。
