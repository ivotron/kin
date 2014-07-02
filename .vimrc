augroup LOCAL_SETUP
  " open tagbar for every go file
  autocmd FileType go nested :TagbarOpen
augroup end

let g:ctrlp_custom_ignore = {
  \ 'dir':  '\v[\/]\.(git|hg|svn)$',
  \ 'file': '',
  \ 'link': '',
  \ }
