#include "Evaluator.h"
#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <cmath>

enum class TokenType { NUMBER, OPERATOR, PAREN };

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType t, const std::string& val) : type(t), value(val) {}
};

int getPrecedence(const std::string& op) {
    if (op == "!" || op == "++" || op == "--" || op == "-u") return 8;
    if (op == "^") return 7;
    if (op == "*" || op == "/" || op == "%") return 6;
    if (op == "+" || op == "-") return 5;
    if (op == ">" || op == "<" || op == ">=" || op == "<=") return 4;
    if (op == "==" || op == "!=") return 3;
    if (op == "&&") return 2;
    if (op == "||") return 1;
    return 0;
}

std::vector<Token> tokenize(const std::string& expr) {
    std::vector<Token> tokens;
    int i = 0;
    TokenType prevType = TokenType::OPERATOR; 

    while (i < expr.length()) {
        char c = expr[i];

        if (isspace(c)) {
            ++i;
            continue;
        }

        if (isdigit(c)) {
            std::string num;
            while (i < expr.length() && isdigit(expr[i])) {
                num += expr[i++];
            }
            tokens.emplace_back(TokenType::NUMBER, num);
            prevType = TokenType::NUMBER;
        } else if (c == '(' || c == ')') {
            tokens.emplace_back(TokenType::PAREN, std::string(1, c));
            prevType = TokenType::PAREN;
            ++i;
        } else {
            std::string op;
            op += c;

            if (i + 1 < expr.length()) {
                std::string twoChar = op + expr[i + 1];
                if (twoChar == "&&" || twoChar == "||" || twoChar == "==" ||
                    twoChar == "!=" || twoChar == "<=" || twoChar == ">=" ||
                    twoChar == "++" || twoChar == "--") {
                    op = twoChar;
                    ++i;
                }
            }

            
            if ((op == "-" && (tokens.empty() || prevType == TokenType::OPERATOR || 
                (prevType == TokenType::PAREN && tokens.back().value == "("))) || 
                op == "++" || op == "--" || op == "!") {

                if (op == "-") op = "-u";
            }

            tokens.emplace_back(TokenType::OPERATOR, op);
            prevType = TokenType::OPERATOR;
            ++i;
        }
    }

    return tokens;
}


std::vector<Token> toPostfix(const std::vector<Token>& tokens) {
    std::vector<Token> output;
    std::stack<Token> ops;

    for (const Token& token : tokens) {
        if (token.type == TokenType::NUMBER) {
            output.push_back(token);
        } else if (token.type == TokenType::OPERATOR) {
            while (!ops.empty() && ops.top().type == TokenType::OPERATOR &&
                   getPrecedence(ops.top().value) >= getPrecedence(token.value)) {
                output.push_back(ops.top());
                ops.pop();
            }
            ops.push(token);
        } else if (token.value == "(") {
            ops.push(token);
        } else if (token.value == ")") {
            while (!ops.empty() && ops.top().value != "(") {
                output.push_back(ops.top());
                ops.pop();
            }
            if (!ops.empty() && ops.top().value == "(") ops.pop(); 
        }
    }

    while (!ops.empty()) {
        output.push_back(ops.top());
        ops.pop();
    }

    return output;
}

int applyOperator(const std::string& op, int a, int b) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw std::runtime_error("Division by zero");
        return a / b;
    }
    if (op == "%") return a % b;
    if (op == "^") return pow(a, b);
    if (op == ">") return a > b;
    if (op == "<") return a < b;
    if (op == ">=") return a >= b;
    if (op == "<=") return a <= b;
    if (op == "==") return a == b;
    if (op == "!=") return a != b;
    if (op == "&&") return a && b;
    if (op == "||") return a || b;
    throw std::runtime_error("Unknown binary operator: " + op);
}
int evaluatePostfix(const std::vector<Token>& postfix) {
    std::stack<int> st;

    for (const Token& token : postfix) {
        if (token.type == TokenType::NUMBER) {
            st.push(std::stoi(token.value));
        } else if (token.type == TokenType::OPERATOR) {
            std::string op = token.value;

            if (op == "!" || op == "++" || op == "--" || op == "-u") {
                if (st.empty()) throw std::runtime_error("Missing operand for unary operator");
                int a = st.top(); st.pop();

                if (op == "!") st.push(!a);
                else if (op == "++") st.push(a + 1);
                else if (op == "--") st.push(a - 1);
                else if (op == "-u") st.push(-a);
            } else {
                if (st.size() < 2) throw std::runtime_error("Missing operands for binary operator");
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(applyOperator(op, a, b));
            }
        }
    }

    if (st.size() != 1) throw std::runtime_error("Invalid postfix evaluation");
    return st.top();
}


// main func that run tokenize, toPostfix, and evaluatePostfix
int Evaluator::eval(const std::string& expression) {
    std::vector<Token> tokens = tokenize(expression);
    std::vector<Token> postfix = toPostfix(tokens);

    std::cout << "Postfix: ";
    for (const Token& token : postfix) {
        std::cout << token.value << " ";
    }
    std::cout << std::endl;

    int result = evaluatePostfix(postfix);
    return result;
}

