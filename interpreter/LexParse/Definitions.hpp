
class Expression{
  public: 
    Expr expression;
    Expression::Expression(Expr expression);
    Expression::Expression(Expression &&to_move);

};

class Print{
  public: 
    Expr expression;
    Print::Print(Expr expression);
    Print::Print(Print &&to_move);

};
