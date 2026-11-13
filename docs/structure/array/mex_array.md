---
title: Point Update MEX Array (一点更新MEX配列)
documentation_of: ../../../src/structure/array/mex_array.hpp
---

符号付き整数の配列を一点更新しながら、配列全体の MEX（含まれない最小の非負整数）を管理する。

## コンストラクタ

```cpp
explicit MexArray(std::vector<std::int64_t> values)
```

`values` を初期配列として構築する。

## set / operator[]

```cpp
void set(std::size_t index, std::int64_t value)
const std::int64_t& operator[](std::size_t index) const
```

`index` 番目の要素の更新と参照を行う。

## mex / size

```cpp
std::size_t mex() const
std::size_t size() const
```

配列全体の MEX と配列の長さを返す。

## 時間計算量

配列の長さを $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N)$ |
| `set` | $O(\log N)$ | $O(1)$ |
| `mex` | $O(\log N)$ | $O(1)$ |
| `operator[]`, `size` | $O(1)$ | $O(1)$ |

## 注意点

- MEX は必ず $[0,N]$ に入るので、それ以外の値（負の値や $N$ より大きい値）は配列には保存するが、MEX の計算用の木には登録しない。
- `set` と `operator[]` は範囲外の添字に対して `std::out_of_range` を投げる。
