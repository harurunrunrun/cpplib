---
title: Point Update MEX Array (一点更新MEX配列)
documentation_of: ../../../src/structure/array/mex_array.hpp
---

符号付き整数配列を一点更新しながら配列全体の MEX を管理する。

## constructor

```cpp
explicit MexArray(vector<int64_t> values)
```

初期配列を構築する。

## set / operator[]

```cpp
void set(size_t index, int64_t value)
const int64_t& operator[](size_t index) const
```

要素の更新と参照を行う。

## mex / size

```cpp
size_t mex() const
size_t size() const
```

配列全体の MEX と長さを返す。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| constructor | $O(N\log N)$ | $O(N)$ |
| `set` | $O(\log N)$ | $O(1)$ |
| `mex` | $O(\log N)$ | $O(1)$ |
| `operator[]`, `size` | $O(1)$ | $O(1)$ |

## 注意点

MEX は `[0,N]` に入るため、それ以外の値は保存するが MEX 木には登録しない。範囲外添字は `out_of_range` を送出する。
