---
title: Encoded Dictionary Segmentation Count (符号化辞書分割数) [MORSE]
documentation_of: ../src/algorithm/string/count_encoded_dictionary_segmentations.hpp
---

符号化済みの文字列を、符号化済み辞書要素の列へ分割する方法数を数えます。
符号化方式そのものは扱わないため、任意のバイト列に利用できます。

## API

### `count_encoded_dictionary_segmentations(encoded_text, encoded_tokens, count_limit)`

```cpp
long long count_encoded_dictionary_segmentations(
    const string& encoded_text,
    const vector<string>& encoded_tokens,
    long long count_limit = numeric_limits<long long>::max()
);
```

辞書要素を順に連結して `encoded_text` と一致させる方法数と
`count_limit` の小さい方を返します。同じ符号列が辞書に複数回現れる場合は、
それぞれを別の選択肢として数えます。

## 引数・戻り値

- `encoded_text`: 分割対象の符号列。空でも構いません。
- `encoded_tokens`: 使用できる非空の符号列。文字集合に制限はありません。
- `count_limit`: 計数を打ち切る非負の上限。
- 戻り値: 分割方法数を `count_limit` で飽和させた値。

## API別の時間計算量・空間計算量

$S=|encoded\_text|$、辞書要素の総長を $D$、最大長を $L$、
byte alphabetの大きさを $\sigma\le 256$、辞書要素がtext中に現れる総数を $z$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_encoded_dictionary_segmentations` | 固定byte alphabetで $O(D+S+z)$ | $O(D+S)$ |

一般の $\sigma$ も変数として数えると、疎な遷移表とmemoized failure遷移の構築・検索を
含めて時間 $O(\sigma(D+S)+z)$、空間 $O(\sigma D+S)$ である。各
`(trie node, byte)` のfailure遷移は初回だけ計算し、以後cacheする。

Aho--Corasick automatonで全辞書要素の出現を終端位置順に列挙し、各出現
`[start,end)` に対して `ways[end] += ways[start] * multiplicity` を行う。
開始位置ごとにtrieを最長要素長まで再走査することはない。

## 注意点

- 空の辞書要素または負の `count_limit` には `invalid_argument` を送出します。
- trieのノード数または同一要素の重複数を内部型で表せない場合は、
  `length_error` または `overflow_error` を送出します。
