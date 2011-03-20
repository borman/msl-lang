structure Parser =
struct
  datatype expr = INTEGER of int
                | REAL of real
                | LITERAL of string
                | VAR of string
                | FUNCCALL of string*expr
                | ARRAYITEM of string*expr
                | TUPLE of expr list
                | COND of expr*expr*expr
                | EXPR of expr
                | INFIX of LexerDef.operation*expr*expr
                | FORMULA 

  datatype operator = DO of expr
                    | LET of expr*expr
                    | IF of expr*operator list
                    | IFELSE of expr*operator list*operator list
                    | WHILE of expr*operator list
                    | FOR of string*expr*expr*operator list

  type function = {name: string, arg: expr, body: operator list}

  fun isLExpr expr = true


  (* ----- SIMPLE EXPRESSION (no infix operators) ----- *)
  fun readSExpr (LexerDef.INTEGER i::rest) = (INTEGER i, rest)
    | readSExpr (LexerDef.REAL r::rest) = (REAL r, rest)
    | readSExpr (LexerDef.LITERAL l::rest) = (LITERAL l, rest)
    | readSExpr (LexerDef.VNAME v::rest) = (VAR v, rest)

    (* FUNCCALL: fname sexpr *)
    | readSExpr (LexerDef.FNAME func::rest) =
  let
    val (arg, rest') = readSExpr rest
  in
    (FUNCCALL (func, arg), rest')
  end

    (* ARRAYITEM: aname sexpr *)
    | readSExpr (LexerDef.ANAME array::rest) =
  let
    val (index, rest') = readSExpr rest
  in
    (ARRAYITEM (array, index), rest')
  end

    (* IF expr THEN expr ELSE expr *)
    | readSExpr (LexerDef.IF::rest) =
  let
    val (cond, LexerDef.THEN::rest') = readExpr rest
    val (then_clause, LexerDef.ELSE::rest'') = readExpr rest'
    val (else_clause, rest''') = readExpr rest''
  in
    (COND (cond, then_clause, else_clause), rest''')
  end

    (* SUBEXPR: (expr) *)
    | readSExpr (LexerDef.LPAREN::rest) =
  let
    val (expr, LexerDef.RPAREN::rest') = readExpr rest
  in
    (EXPR expr, rest')
  end

    (* TUPLE: [expr, expr, expr] *)
    | readSExpr (LexerDef.LBRACKET::rest) =
  let
    fun readNext (acc, tokens) = 
    let
      val (expr, rest) = readExpr tokens
    in
      case rest of
           LexerDef.COMMA::rest' => readNext (expr::acc, rest')
         | LexerDef.RBRACKET::rest' => (List.rev (expr::acc), rest')
    end

    val (exprs, rest') = readNext ([], rest)
  in
    (TUPLE exprs, rest')
  end


  (* ----- GENERAL EXPRESSION ----- *)
  and readExpr tokens = 
  let
    datatype formula_item = F_EXPR of expr
                          | F_OP of LexerDef.operation

    fun readNext (acc, tokens) = 
    let
      val (expr, rest) = readSExpr tokens
    in
      case rest of
           LexerDef.OP oper::rest' => readNext (F_OP oper::F_EXPR expr::acc, rest')
         | _ => (List.rev (F_EXPR expr::acc), rest)
    end

    fun foldFormula ops (F_EXPR l::F_OP oper::F_EXPR r::rest) =
      if List.exists (fn x => x=oper) ops 
      then foldFormula ops (F_EXPR (INFIX (oper, l, r))::rest)
      else F_EXPR l::F_OP oper::foldFormula ops (F_EXPR r::rest)

      | foldFormula ops [F_EXPR expr] = [F_EXPR expr]

    val foldAll = (fn [x] => x)
                o (foldFormula [LexerDef.AND, LexerDef.OR]) 
                o (foldFormula [LexerDef.EQ, LexerDef.LESS, LexerDef.MORE])
                o (foldFormula [LexerDef.PLUS, LexerDef.MINUS])
                o (foldFormula [LexerDef.MUL, LexerDef.DIV, LexerDef.MOD])

    val (exprs, rest) = readNext ([], tokens)
    val F_EXPR expr = foldAll exprs 
    (* val expr = FORMULA *)
  in
    (expr, rest)
  end


  (* ----- OPERATOR ----- *)
  (* DO expr *)
  and readOperator (LexerDef.DO::rest) =
  let
    val (expr, rest') = readExpr rest
  in
    (DO expr, rest')
  end

  (* IF expr THEN block*)
    | readOperator (LexerDef.IF::rest) =
  let
    val (cond, LexerDef.THEN::rest') = readExpr rest
    val (body, rest'') = readBlock rest'
  in
    case rest'' of
         LexerDef.ELSE::rest''' =>
         let
           val (else_body, rest'''') = readBlock rest'''
         in
           (IFELSE (cond, body, else_body), rest'''')
         end
       | _ => (IF (cond, body), rest'')
  end

  (* WHILE expr block*)
    | readOperator (LexerDef.WHILE::rest) =
  let
    val (cond, rest') = readExpr rest
    val (body, rest'') = readBlock rest'
  in
    (WHILE (cond, body), rest'')
  end

  (* FOR vname FROM expr TO expr *)
    | readOperator (LexerDef.FOR::LexerDef.VNAME iter::LexerDef.FROM::rest) =
  let
    val (from, LexerDef.TO::rest') = readExpr rest
    val (to, rest'') = readExpr rest'
    val (body, rest''') = readBlock rest''
  in
    (FOR (iter, from, to, body), rest''')
  end

  (* LET: lexpr = expr *)
    | readOperator tokens =
  let
    val (lexpr, LexerDef.OP EQ::rest) = readSExpr tokens
    val true = isLExpr lexpr
    val (rexpr, rest') = readExpr rest
  in
    (LET (lexpr, rexpr), rest')
  end
      

  (* ----- BLOCK ----- *)
  and readBlock (LexerDef.END::rest) = ([], rest)
    | readBlock tokens =
  let
    val (operator, tokens') = readOperator tokens
    val (block_rest, tokens'') = readBlock tokens'
  in
    (operator::block_rest, tokens'')
  end


  (* ----- FUNCTION ----- *)
  and readFun tokens =
  let
    val LexerDef.FUN :: LexerDef.FNAME fname :: tokens' = tokens
    val (arg, tokens'') = readExpr tokens'
    val true = isLExpr arg
    val (body, tokens''') = readBlock tokens''
  in
    ({name=fname, arg=arg, body=body}, tokens''')
  end


  (* ----- TOP-LEVEL ----- *)
  fun parse [] = []
    | parse tokens =
  let
    val (func, tokens') = readFun tokens
  in
    func :: parse tokens'
  end
  
end
