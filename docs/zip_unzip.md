---
title: Zip Unzip
documentation_of: ../src/algorithm/other/zip_unzip.hpp
---

配列をソースコードに埋め込みやすい形にするための run-length zip/unzip。

# 関数

```cpp
run_length_zip(data)
run_length_unzip(data)
```

`run_length_zip` は連続する同じ値を `(value, count)` にまとめる。

## 時間計算量

- $O(N)$

## API別計算量

$N$ を展開後の要素数、$R$ をrun数とする。

| API | 時間計算量 | 出力領域 |
| --- | --- | --- |
| `run_length_zip(data)` | $O(N)$ | $O(R)$ |
| `run_length_unzip(data)` | $O(R+N)$ | $O(N)$ |

`run_length_unzip` は最初に全 $R$ runの長さを検査し、その後 $N$ 要素を列挙する。
