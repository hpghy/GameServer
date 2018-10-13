set nocompatible
filetype off		" required
set rtp+=~/.vim/bundle/vundle/
call vundle#rc()
" let Vundle manage Vundle
Bundle 'gmarik/vundle'
"my Bundle here:
"
" original repos on github
Bundle 'Valloric/YouCompleteMe'
Bundle 'jiangmiao/auto-pairs'

filetype plugin indent on

filetype on
syntax on 
set nocompatible
set number
set hlsearch
set history=1000 
set noignorecase
set enc=utf8 
set fencs=utf8,gbk,gb2312,gb18030,cp936 
set autoindent
set smartindent
set tabstop=4
set shiftwidth=4
set expandtab ts=4 sw=4 sts=4
set showmatch

let mapleader=","
nnoremap <leader>gl :YcmCompleter GoToDeclaration<CR>     " 逗号+g+l
nnoremap <leader>gf :YcmCompleter GoToDefinition<CR>      " 逗号+g+f
nnoremap <leader>gg :YcmCompleter GoToDefinitionElseDeclaration<CR>  " 逗号+g+g
