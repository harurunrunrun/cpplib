---
title: Zip Unzip (圧縮・展開)
documentation_of: ../../../../src/algorithm/other/utility/zip_unzip.hpp
---

配列をソースコードに埋め込みやすくするための、ランレングス圧縮と展開。

## 関数

```cpp
template<class T>
vector<pair<T, int>> run_length_zip(const vector<T>& data);

template<class T>
vector<T> run_length_unzip(const vector<pair<T, int>>& data);
```

`run_length_zip` は連続する等しい値（`==` で比較）を `(value, count)` にまとめる。`run_length_unzip` はその逆変換を行う。

## 時間計算量・空間計算量

$N$ を展開後の要素数、$R$ をラン数とする。

| API | 時間計算量 | 出力領域 |
| --- | --- | --- |
| `run_length_zip(data)` | $O(N)$ | $O(R)$ |
| `run_length_unzip(data)` | $O(R+N)$ | $O(N)$ |

`run_length_unzip` は最初に全 $R$ 個のランの長さを検査し、その後 $N$ 要素を列挙する。

## 注意点

- `run_length_unzip` に負の長さを渡すと `runtime_error` を送出する。長さ $0$ のランは許す。
- 展開後の総要素数は `int` で表現できなければならない（内部で `int` に合計する）。
- 空入力には空の列を返す。
