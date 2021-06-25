
Binary::Binary(Expr* left, Token operator, Expr* right) {
  this->left = left;
  this->operator = operator;
  this->right = right;
}

Grouping::Grouping(Expr* expression) {
  this->expression = expression;
}

Literal::Literal(void* value) {
  this->value = value;
}

Unary::Unary(Token operator, Expr* right) {
  this->operator = operator;
  this->right = right;
}
