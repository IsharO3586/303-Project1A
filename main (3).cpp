#include <iostream>
#include <vector>
#include "Evaluator.h"

// test serveral expressions



int main() {
    Evaluator eval;
  // list of expressions to test
    std::vector<std::string> tests = {
        "1 + 2 * 3",
        "(1 + 2) * 3",
        "!!!1",
        "3 * -3",
        "1 + 2^2 * 3",
        "1 + 3 > 2",
        "(4 >= 4) && 0"
    };

    // loop through each test expresion
    for (const auto& expr : tests) {
        try {
            int result = eval.eval(expr);
            std::cout << "Expr: " << expr << " => Result: " << result << "\n";
        } catch (const std::exception& e) {
            std::cout << "Expr: " << expr << " => Error: " << e.what() << "\n";
        }
    }

    return 0;
}
