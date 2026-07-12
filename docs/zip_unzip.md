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
