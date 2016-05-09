"" Ubuntu Version of Daniel's vimrc

"" Vim settings, rather than Vi settings (must be first)
set nocompatible

"" -------- VUNDLE STUFF -----------
set nocompatible
filetype off                  " required

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()

" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'

" Keep Plugin commands between vundle#begin/end.
" plugin on GitHub repo
Plugin 'Valloric/YouCompleteMe'

" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required
"" ----------------------------- /VUNDLE

"" ----------- YCM STUFF ------
nnoremap <C-]> <C-]>:YcmCompleter GoTo<CR>
"" ---------------------------- /YCM

"" Unicode Support
scriptencoding utf-8
set encoding=utf-8

"" Behavior
" autocmd BufRead * loadview
" autocmd BufWritePost * mkview
" autocmd BufWritePost * silent execute '! if [ -f tags ]; then ctags -R .; fi'
set tags=./tags,tags;$HOME
set backup
set backupdir=~/vimfiles/backup,./.backup
set undofile
set undodir=~/vimfiles/backup,./.backup
set dir=~/vimfiles/swap
behave xterm
set backspace=indent,eol,start
set fdm=marker
set tabstop=2
set shiftwidth=2
set autoindent
set expandtab
set linebreak
set nowrap
set wildmenu
set incsearch
set hlsearch

"" force yourself to stop using the arrow keys:
map <up>    <nop>
map <down>  <nop>
map <left>  <nop>
map <right> <nop>
" harder
imap <up>    <nop>
imap <down>  <nop>
imap <left>  <nop>
imap <right> <nop>

"" GUI
if has("gui_running")
set laststatus=2
set lines=45
set columns=112
color twilight
let g:sessionfontsize=8
set guifont=Consolas:h8:cANSI "remember to also change the font keymaps!
set guioptions=eg
endif
syntax on
set listchars=trail:·,precedes:«,extends:»,tab:▸\ ,eol:▾
set whichwrap+=<,>,[,]
set nu
set rnu
set list
set display +=lastline
autocmd FileType c,cpp,java,scala set colorcolumn=80
autocmd FileType vim              set colorcolumn=80
autocmd FileType text             set colorcolumn=0
hi ColorColumn ctermbg=darkgrey guibg=#333333
" SECTION: GUI Functions {{{1
function! ToggleFullscreen()
	if matchstr(&guioptions,"m")!=""
		set guioptions -=b
		set guioptions -=m
		set guioptions -=r
		set guioptions -=L
		return "Fullscreen On"
	else
		set guioptions +=b
		set guioptions +=m
		set guioptions +=r
		set guioptions +=L
		return "Fullscreen Off"
	endif
endfunction
function! ToggleDocType()
	if &l:wrap
		setl nowrap
		setl nu
        setl rnu
		setl list
        setl textwidth=78
		return "Code Document"
	else
		setl wrap
		setl nonu
        setl nornu
		setl nolist
        setl textwidth=0
		return "Text Document"
	endif
endfunction
function! IncrementFontSize()
    let g:sessionfontsize+=1
    exec "set guifont=Consolas:h" . g:sessionfontsize . ":cANSI"
endfunction
function! DecrementFontSize()
    let g:sessionfontsize-=1
    exec "set guifont=Consolas:h" . g:sessionfontsize . ":cANSI"
endfunction
" }}}1

"" Keymaps
inoremap jj <Esc>
let mapleader = "\<Space>"
noremap <Leader>w <C-W>
noremap <Leader>ya magg"+yG`a
noremap <F2>		:echo ToggleDocType()<CR>
noremap <F11>		:echo ToggleFullscreen()<CR>
nnoremap <M-Down> :cd %:p:h<CR>
nnoremap <M-Up> :cd %:p:h:h<CR>
nnoremap <Leader><Down> :cd %:p:h<CR>
nnoremap <Leader><Up> :cd %:p:h:h<CR>
" Alt-+ Alt-- zoom in and out TODO: do this better, add L and H support
nnoremap <silent> <leader><M-->
            \ :set guifont=Consolas:h2:cANSI<CR>
            \:let &columns = &columns*3<CR>:let &lines = 40+(&lines*2)<CR>
            \:set guioptions+=r<CR>
            \:nnoremap j 10j<CR>
            \:nnoremap k 10k<CR>
nnoremap <silent> <leader><M-=>
            \ :let &columns = &columns/3<CR>:let &lines = (&lines-40)/2<CR>
            \:set guifont=Consolas:h8:cANSI<CR>
            \:set guioptions-=r<CR>
            \:nnoremap j j<CR>
            \:nnoremap k k<CR>
nnoremap <silent> <M--> :echo DecrementFontSize()<CR>
nnoremap <silent> <M-=> :echo IncrementFontSize()<CR>
" Commenting blocks of code.
autocmd FileType c,cpp,java,scala let b:comment_leader = '//'
autocmd FileType sh,ruby,python   let b:comment_leader = '#'
autocmd FileType conf,fstab       let b:comment_leader = '#'
autocmd FileType tex              let b:comment_leader = '%'
autocmd FileType mail             let b:comment_leader = '>'
autocmd FileType vim              let b:comment_leader = '"'
vnoremap <Leader>c :s:^:<C-R>=b:comment_leader<CR> 
            \:<CR>:silent!'<,'>s:^<C-R>=b:comment_leader<CR> <C-R>=b:comment_leader<CR> 
            \:<CR>:noh<CR>`<v`>
nnoremap <Leader>c V:s:^:<C-R>=b:comment_leader<CR> 
            \:<CR>:silent!'<,'>s:^<C-R>=b:comment_leader<CR> <C-R>=b:comment_leader<CR> 
            \:<CR>:noh<CR>`<
" Space-n toggles relative line numbering
nnoremap <Leader>n :set rnu!<CR>:set rnu?<CR>
" Alt-n and N for :cnext :cprev
nnoremap <M-*> *:exec "vim // ".escape(expand("%:p:h"),' ')."/*.*"<CR>
nnoremap <M-n> :cnext<CR>
nnoremap <M-N> :cprev<CR>
" In the help window: Tab and shift-Tab switch between searched topics
" SECTION: Help functions {{{1
function! NextHelp()
    exec "normal :h\<Up>\<CR>"
    if !exists("w:count_help")
        let w:count_help=1
    endif
    if !exists("w:origin_help")
        let w:origin_help="impossiblestring"
    endif
    let w:last_help=strpart(@:,1+match(@:," "))
    if match(w:last_help, w:origin_help)==-1
        let w:origin_help=w:last_help
        let w:count_help=1
    else
        let w:count_help+=1
    endif
    set wcm=<Tab>
    exec "normal :h ".w:origin_help.repeat("\<Tab>",w:count_help)."\<CR>"
endfunction
function! PrevHelp()
    exec "normal :h\<Up>\<CR>"
    if !exists("w:count_help")
        let w:count_help=1
    endif
    if !exists("w:origin_help")
        let w:origin_help="impossiblestring"
    endif
    let w:last_help=strpart(@:,1+match(@:," "))
    if match(w:last_help, w:origin_help)==-1
        let w:origin_help=w:last_help
        let w:count_help=1
    elseif w:count_help>1
        let w:count_help-=1
    endif
    set wcm=<Tab>
    exec "normal :h ".w:origin_help.repeat("\<Tab>",w:count_help)."\<CR>"
endfunction
" }}}1
autocmd BufEnter * silent exec "normal :if &bt=='help'\<CR>nnoremap <buffer> <Tab> :silent echo NextHelp()<CR>\<CR>endif\<CR>"
autocmd BufEnter * silent exec "normal :if &bt=='help'\<CR>nnoremap <buffer> <s-Tab> :silent echo PrevHelp()<CR>\<CR>endif\<CR>"
" Extra Cursor Half-Screen Movement
nnoremap <silent> H :silent if winline() != 1<CR>
            \normal! H<CR>else<CR>
            \exec "normal ".
            \"zz"<CR>
            \end<CR><CR>
"             \string((winheight(winnr())==1)+winheight(winnr())/2)."kzz"<CR>
nnoremap <silent> L :silent if winline() != winheight(winnr())<CR>
            \normal! L<CR>else<CR>
            \exec "normal ".
            \"zz"<CR>
            \end<CR><CR>
"             \string((winheight(winnr())==1)+winheight(winnr())/2)."jzz"<CR>
" nnoremap <silent> L :silent if winheight(winnr()) != winline()<CR>
"             \normal! L<CR>else<CR>normal zzM<CR>end<CR><CR>

"" vim: set fdm=marker:
