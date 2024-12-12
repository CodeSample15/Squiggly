#include "external-libs.hpp"
#include "../lib/exprtk/exprtk.hpp"

//for converting string representations of expressions into numbers
exprtk::parser<double> expParser;
exprtk::expression<double> expExpression;

double expressionToDouble(std::string exp) {
    expParser.compile(exp, expExpression);
    return expExpression.value();
}