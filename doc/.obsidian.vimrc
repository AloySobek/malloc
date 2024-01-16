" Insert mode keymaps
imap kj <Esc>

" Normal mode keymaps
nnoremap U <C-r>
nnoremap J }
nnoremap K {
nnoremap H b
nnoremap L w
nnoremap ,y "+y
nnoremap ,Y "+yg_
nnoremap ,yy "+yy
nnoremap ,p "+p
nnoremap ,P "+P
nnoremap ,r :%s/\\<<C-r><C-w>\\>//g<left><left>

" Visual mode keymaps
vnoremap J }
vnoremap K {
vnoremap H b
vnoremap L w
vnoremap ,y "+y
vnoremap ,p "+p
vnoremap ,P "+P
