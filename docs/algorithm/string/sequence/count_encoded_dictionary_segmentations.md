---
title: Encoded Dictionary Segmentation Count (符号化辞書分割数) [MORSE]
documentation_of: ../../../../src/algorithm/string/sequence/count_encoded_dictionary_segmentations.hpp
---

符号化済みの文字列を、符号化済みの辞書要素の列に分割する方法の数を数える。
符号化方式そのものは扱わないため、任意のバイト列に利用できる。

## API

### `count_encoded_dictionary_segmentations(encoded_text, encoded_tokens, count_limit)`

```cpp
long long count_encoded_dictionary_segmentations(
    const string& encoded_text,
    const vector<string>& encoded_tokens,
    long long count_limit = numeric_limits<long long>::max()
);
```

辞書要素を順に連結して `encoded_text` と一致させる方法の数と、
`count_limit` の小さい方を返す。同じ符号列が辞書に複数回現れる場合は、
それぞれを別の選択肢として数える。

## 引数・戻り値

- `encoded_text`: 分割する符号列。空でもよい。
- `encoded_tokens`: 使用できる空でない符号列。文字集合に制限はない。
- `count_limit`: 計数を打ち切る非負の上限。
- 戻り値: 分割方法の数を `count_limit` で飽和させた値。

## 時間計算量・空間計算量

$S$ を `encoded_text` の長さ、辞書要素の長さの合計を $D$、
バイトのアルファベットの大きさを $\sigma\le 256$、辞書要素がテキスト中に現れる総数を $z$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_encoded_dictionary_segmentations` | アルファベットを定数とみなして $O(D+S+z)$ | $O(D+S)$ |

$\sigma$ も変数として数えると、疎な遷移表とメモ化した失敗遷移の構築・検索を
含めて時間 $O(\sigma(D+S)+z)$、空間 $O(\sigma D+S)$ である。
各 (トライのノード, バイト) の組の失敗遷移は初回だけ計算し、以後はキャッシュする。

Aho--Corasick オートマトンで全辞書要素の出現を終端位置の順に列挙し、各出現
`[start, end)` に対して `ways[end] += ways[start] * multiplicity` を行う。
開始位置ごとにトライを最長要素長まで再走査することはない。

## 注意点

- 空の辞書要素または負の `count_limit` には `std::invalid_argument` を送出する。
- トライのノード数または同一要素の重複数を内部の型で表せない場合は、
  `std::length_error` または `std::overflow_error` を送出する。
