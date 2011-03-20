structure LexerDef = LexerImpl.UserDeclarations

structure Lexer = 
struct
  type lexem = LexerDef.lexresult

  fun lexFile filename =
  let
    val file = TextIO.openIn filename
    val lexer = LexerImpl.makeLexer (fn i => TextIO.inputN (file, i))
    fun do_lex acc =
      case lexer () of
           LexerDef.EOF => acc
         | token => do_lex (token::acc)
    val lex_result = do_lex []
    val () = TextIO.closeIn file
  in
    List.rev lex_result
  end
end
