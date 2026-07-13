---
title: Fast IO
documentation_of: ../src/structure/other/fastio.hpp
---

`stdin` と `stdout` をそれぞれ固定長bufferで扱う整数・文字列入出力。
destructorで未出力bufferをflushするため、通常はmain関数の先頭で構築する。

```cpp
fastio io;
int n;
long long x;
char word[100];
io.readint(n);
io.readll(x);
io.readstr(word);
io.writeint(n);
io.write(' ');
io.writell(x);
io.write('\n');
```

## API

| 操作 | 説明 | 時間計算量 |
| --- | --- | --- |
| `fastio()` | 空の入出力bufferを構築する | $O(1)$ |
| `~fastio()` | 未出力bufferを`stdout`へ書き出す | buffer内の文字数に対して$O(B)$ |
| `readint(int& x)` | 次の空白区切り10進整数を`x`へ格納する | 読み込む文字数に対して$O(D)$ |
| `readll(long long& x)` | 次の空白区切り10進整数を`x`へ格納する | $O(D)$ |
| `readstr(char* s)` | 次の空白区切り文字列と終端`'\0'`を`s`へ格納する | $O(L)$ |
| `write(char c)` | 1文字を出力bufferへ追加する | amortized $O(1)$ |
| `write(const char* s)` | 終端`'\0'`まで追加する | $O(L)$ |
| `writeint(int x)` | `x`を10進表記で追加する | $O(D)$ |
| `writell(long long x)` | `x`を10進表記で追加する | $O(D)$ |

## 前提・容量

- `readstr`の出力先には文字列長+1以上の領域を用意する。
- 数値入力は対象型で表現できる10進整数とする。入力失敗を通知する戻り値や例外はない。
- `writeint`と`writell`に最小値を渡すと絶対値を同じ符号付き型で表現できないため、対象外とする。
- 読み書きbufferはそれぞれ`read_size`, `write_size` byte。bufferの補充・flushを含めても全体は入出力文字数に対して線形時間。
