" clang_complete
augroup LOCAL_SETUP
  " update on save
  autocmd BufWritePost *.c,*.cpp,*.cxx,*.cc,*.hxx,*.h call g:ClangUpdateQuickFix()
  autocmd BufRead,BufNewFile *.c,*.cpp,*.cxx,*.cc,*.hxx,*.h setlocal tw=110 | setlocal ts=2 | setlocal sw=2 | setlocal expandtab
  autocmd FileType go nested :TagbarOpen
augroup end

setlocal tw=100

let g:ctrlp_custom_ignore = {
  \ 'dir':  'src\/test\/gmock\|build\|\v[\/]\.(git|hg|svn)$',
  \ 'file': '\v\.(exe|so|dll)$',
  \ 'link': 'some_bad_symbolic_links',
  \ }
