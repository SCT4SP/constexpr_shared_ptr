```
pdflatex P3037R5.tex
pdftk A=P3037R5.pdf B=std-diff.pdf cat A1-6 B A33-end output output.pdf
mv output.pdf generated/P30375.pdf
```

```
# sudo apt-get install curl pandoc python3-venv python3-requests texlive-xetex
# git clone https://github.com/mpark/wg21.git $HOME/code/ext/wg21
# cd $HOME/code/ext/wg21
# make update           # "to update the local cache of annex-f"
# make
# cd -
make -f $HOME/code/ext/wg21/Makefile PXXXXR0.pdf
```
