#!/bin/bash

name=P3037R5
tmp_name=tmp-${name}
m4 ${name}.md > ${tmp_name}.md
make -f $HOME/code/ext/wg21/Makefile ${tmp_name}.pdf
mv generated/${tmp_name}.pdf generated/${name}.pdf
rm ${tmp_name}.md
