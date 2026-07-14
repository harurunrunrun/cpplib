---
title: XOR-Decoded Range Greater Query (XOR復号付き区間超過個数) [KQUERYO]
documentation_of: ../src/structure/wavelet_matrix/xor_decoded_range_greater_query.hpp
---

直前の答えとのXORで復号するオンライン区間queryについて、閾値より大きい要素数を管理する。

# XorDecodedRangeGreaterQuery

```cpp
template<integral T, int MAX_SIZE, int ID_BIT_WIDTH = ...>
class XorDecodedRangeGreaterQuery {
public:
    explicit XorDecodedRangeGreaterQuery(const vector<T>& values);

    int size() const;
    int last_answer() const;
    int query(int encoded_left, int encoded_right, T encoded_threshold);
    void reset();
};
```

## Constructor

静的な値列を構築する。直前の答えは0になる。

## size

値列の長さを返す。

## last_answer

直前の `query` の返り値を返す。構築直後と `reset` 後は0である。

## query

3引数をそれぞれ直前の答えとXORし、区間端点と閾値を復号する。端点は1-indexedの閉区間として、左端を1以上、右端を `size()` 以下へ切り詰める。切り詰めた区間が空なら0、そうでなければ閾値より大きい要素数を返し、直前の答えをその値へ更新する。

## reset

直前の答えを0へ戻す。値列は変更しない。

## API別の時間計算量・空間計算量

値列長を $N$、異なる値数を $D$、`ID_BIT_WIDTH` を $H$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N\log N+H(N+\lceil MAX\_SIZE/64\rceil))$ | $O(HN+H\lceil MAX\_SIZE/64\rceil)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `last_answer()` | $O(1)$ | $O(1)$ |
| `query(...)` | $O(\log D+H)$ | $O(1)$ |
| `reset()` | $O(1)$ | $O(1)$ |

## 注意点

- `T` は整数型でなければならない。
- 値列長と異なる値数は `CompressedWaveletMatrix` の容量・bit幅の前提を満たす必要がある。
- 復号後の端点は上記の規則で切り詰めるため、入力端点自体に範囲前提はない。
