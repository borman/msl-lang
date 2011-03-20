datatype operation = EQ | LESS | MORE
                   | PLUS | MINUS | MUL | DIV | MOD 
                   | AND | OR
datatype lexresult = VNAME of string
                   | FNAME of string
                   | ANAME of string
                   | INTEGER of int
                   | REAL of real
                   | LITERAL of string
                   | BOOL of bool
                   | DO | FUN | END | FOR | FROM | TO | WHILE | IF | THEN | ELSE
                   | LBRACKET | RBRACKET | LPAREN | RPAREN
                   | COMMA
                   | OP of operation
                   | EOF
val eof = fn() => EOF
%%
%structure LexerImpl
TokenSym=[a-zA-Z0-9\$_];
Whitespace=[\ \t\n];
%%
(\;.*) | ([\ \t\n]+) => (lex());

do => (DO);
fun => (FUN);
end => (END);
for => (FOR);
from => (FROM);
to => (TO);
while => (WHILE);
if => (IF);
then => (THEN);
else => (ELSE);

true | false => (BOOL (yytext="true"));
and => (OP AND);
or => (OP OR);

[A-Z]{TokenSym}* => (VNAME yytext);
[a-z]{TokenSym}* => (FNAME yytext);
\$[A-Z]{TokenSym}* => (ANAME (String.extract (yytext, 1, NONE)));
~?[0-9]+ => (INTEGER (Option.valOf (Int.fromString yytext)));
~?[0-9]*\.[0-9]+ => (REAL (Option.valOf (Real.fromString yytext)));
\".*\" => (LITERAL (String.substring (yytext, 1, String.size yytext - 2)));

"[" => (LBRACKET);
"]" => (RBRACKET);
"(" => (LPAREN);
")" => (RPAREN);

"," => (COMMA);

"=" => (OP EQ);
"<" => (OP LESS);
">" => (OP MORE);
"+" => (OP PLUS);
"-" => (OP MINUS);
"*" => (OP MUL);
"/" => (OP DIV);
"%" => (OP MOD);
