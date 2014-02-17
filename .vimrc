" clang_complete
augroup LOCAL_SETUP
  " update on save
  autocmd BufWritePost *.c,*.cpp,*.cxx,*.cc,*.hxx,*.h call g:ClangUpdateQuickFix()
  autocmd BufRead,BufNewFile *.c,*.cpp,*.cxx,*.cc,*.hxx,*.h setlocal tw=110 | setlocal ts=2 | setlocal sw=2 | setlocal expandtab
augroup end
