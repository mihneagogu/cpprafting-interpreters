
class Binary{
  public: 
    Expr* left;
    Token operator;
    Expr* right;
    Binary::Binary(Expr* left, Token operator, Expr* right);
    Binary::Binary(Binary &&to_move);

};

class Grouping{
  public: 
    Expr* expression;
    Grouping::Grouping(Expr* expression);
    Grouping::Grouping(Grouping &&to_move);

};

class Literal{
  public: 
    void* value;
    Literal::Literal(void* value);
    Literal::Literal(Literal &&to_move);

};

class Unary{
  public: 
    Token operator;
    Expr* right;
    Unary::Unary(Token operator, Expr* right);
    Unary::Unary(Unary &&to_move);

};
