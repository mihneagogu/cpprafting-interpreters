#ifndef EXPR_H_
#define EXPR_H_

enum ExprTy {

};

class BinaryExpr {

};

class Expr {
    public:
        ExprTy ty;
        union {
            BinaryExpr bin;
        } content;
};


#endif // EXPR_H_
