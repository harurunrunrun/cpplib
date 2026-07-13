---
layout: default
title: cpplib Library (競技プログラミングライブラリ)
---

# cpplib Library (競技プログラミングライブラリ)

このページは、GitHub Pages が一時的にブランチから公開されている場合のライブラリ索引です。
通常の公開では competitive-verifier が生成した索引に置き換わります。

{% assign library_pages = site.pages | sort: "title" %}

## Algorithm (アルゴリズム)

{% for library_page in library_pages %}
{% if library_page.documentation_of contains '../src/algorithm/' %}
- [{{ library_page.title }}]({{ library_page.url | relative_url }})
{% endif %}
{% endfor %}

## Structure (データ構造)

{% for library_page in library_pages %}
{% if library_page.documentation_of contains '../src/structure/' %}
- [{{ library_page.title }}]({{ library_page.url | relative_url }})
{% endif %}
{% endfor %}

## Approximate (近似・ヒューリスティック)

{% for library_page in library_pages %}
{% if library_page.documentation_of contains '../src/approximate/' %}
- [{{ library_page.title }}]({{ library_page.url | relative_url }})
{% endif %}
{% endfor %}
