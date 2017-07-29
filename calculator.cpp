#include <string>
#include <deque>
#include <vector>
#include <exception>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <functional>
#include <algorithm>


class Token {
public:
    enum Type {
        Unknown = 0,
        Number,
        Operator,
        LeftParen,
        RightParen,
        Dot,
    };

    Token(Type t, const std::string& s, int prec = -1, bool ra = false)
        : type{ t }, str(s), precedence{ prec }, rightAssociative{ ra }
    {}

    Type type{ Type::Unknown };
    std::string str{};
    int precedence{ -1 };
    bool rightAssociative{ false };
};

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << token.str;
    return os;
}


// Debug output
template<class T, class U>
void debugOutput(const Token& token, const T& queue, const U& stack, const std::string& comment = "") {
    std::ostringstream ossQueue;
    for (const auto& t : queue) {
        ossQueue << " " << t;
    }

    std::ostringstream ossStack;
    for (const auto& t : stack) {
        ossStack << " " << t;
    }

    printf("|%-3s|%-32s|%10s| %s\n"
        , token.str.c_str()
        , ossQueue.str().c_str()
        , ossStack.str().c_str()
        , comment.c_str()
    );
}

std::deque<Token> ExprToTokens(const std::string& expr) {
    std::deque<Token> tokens;

    for (const auto* p = expr.c_str(); *p; ++p) {
        if (isdigit(*p)) {
            const auto* b = p;
            for (; isdigit(*p); ++p) {}
            const auto s = std::string(b, p);
            tokens.push_back(Token{ Token::Type::Number, s });
            --p;
        }
        else {
            Token::Type t = Token::Type::Unknown;
            int precedence = -1;
            bool rightAssociative = false;
            switch (*p) {
            default:                                    break;
            case '.':   t = Token::Type::Dot;           break;
            case '(':   t = Token::Type::LeftParen;     break;
            case ')':   t = Token::Type::RightParen;    break;
            case '^':   t = Token::Type::Operator;      precedence = 4; rightAssociative = true;  break;
            case '*':   t = Token::Type::Operator;      precedence = 3; break;
            case '/':   t = Token::Type::Operator;      precedence = 3; break;
            case '+':   t = Token::Type::Operator;      precedence = 2; break;
            case '-':   t = Token::Type::Operator;      precedence = 2; break;
            }
            tokens.push_back(Token{
                t, std::string(1, *p), precedence, rightAssociative
            });
        }
    }

    return tokens;
}


std::deque<Token> ShuntingYard(const std::deque<Token>& tokens) {
    std::deque<Token> queue;
    std::vector<Token> stack;
    bool gotOperator = false;

    for (auto token : tokens) {
        switch (token.type) {
        case Token::Type::Number:
        {
            if (!queue.empty()) {
                std::string::iterator it = std::find(queue.back().str.begin(), queue.back().str.end(), '.');
                if (it != queue.back().str.end() && !gotOperator) {
                    queue.back().str += token.str;
                }
                else {
                    queue.push_back(token);
                    gotOperator = false;
                }
                break;
            }
            queue.push_back(token);
            break;
        }
        case Token::Type::Dot: {
            queue.back().str += ".";
            break;
        }

        case Token::Type::Operator:
        {
            const auto o1 = token;

            gotOperator = true;

            while (!stack.empty()) {
                const auto o2 = stack.back();

                // either o1 is left-associative and its precedence is
                // *less than or equal* to that of o2,
                // or o1 if right associative, and has precedence
                // *less than* that of o2,
                if (
                    (!o1.rightAssociative && o1.precedence <= o2.precedence)
                    || (o1.rightAssociative && o1.precedence <  o2.precedence)
                    ) {
                    stack.pop_back();
                    queue.push_back(o2);
                    continue;
                }
                break;
            }

            stack.push_back(o1);
        }
        break;

        case Token::Type::LeftParen:
            stack.push_back(token);
            break;

        case Token::Type::RightParen:
        {
            bool match = false;
            while (!stack.empty()) {
                const auto tos = stack.back();
                if (tos.type != Token::Type::LeftParen) {
                    stack.pop_back();
                    queue.push_back(tos);
                }

                // Pop the left parenthesis from the stack,
                // but not onto the output queue.
                stack.pop_back();
                match = true;
                break;
            }

            if (!match && stack.empty()) {
                // If the stack runs out without finding a left parenthesis,
                // then there are mismatched parentheses.
                std::cout << "RightParen error " << token.str.c_str() << std::endl;
                exit(0);
            }
        }
        break;

        default:
            std::cout << "error " << token.str.c_str() << std::endl;
            exit(0);
            break;
        }
#ifdef DEBUG
        debugOutput(token, queue, stack);
#endif // DEBUG
    }

    while (!stack.empty()) {
        if (stack.back().type == Token::Type::LeftParen) {
            std::cout << "Mismatched parentheses error\n";
            exit(0);
        }

        queue.push_back(std::move(stack.back()));
        stack.pop_back();
    }

#ifdef DEBUG
    debugOutput(Token{ Token::Type::Unknown, "End" }, queue, stack);
#endif // DEBUG

    return queue;
}

void RemoveSpaces(std::string& s) {
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
}

void ReplaceCommas(std::string& s) {
    std::replace(s.begin(), s.end(), ',', '.');
}

void FormatString(std::string& s) {
    RemoveSpaces(s);
    ReplaceCommas(s);
}

void calculator(std::string& expr) {

    FormatString(expr);
#ifdef DEBUG
    std::cout << expr << std::endl;
#endif // DEBUG

    std::cout << std::fixed << std::setprecision(2) << std::setw(4);

#ifdef DEBUG
    std::cout << "Shunting-yard\n";
    printf("|%-3s|%-32s|%-10s|\n", "Tkn", "Queue", "Stack");
#endif // DEBUG

    const auto tokens = ExprToTokens(expr);
    auto queue = ShuntingYard(tokens);
    std::vector<double> stack;

#ifdef DEBUG
    std::cout << "\nCalculation\n";
    std::cout << std::setw(3) << "|Tkn|";
    std::cout << std::setw(32) << std::left << "Queue";
    std::cout << std::setw(10) << std::left << "|Stack" << std::endl;
#endif

    while (!queue.empty()) {
        std::string op;

        const auto token = queue.front();
        queue.pop_front();
        switch (token.type) {
        case Token::Type::Number:
            stack.push_back(std::stof(token.str));
            op = "Push " + token.str;
            break;

        case Token::Type::Dot:
            break;

        case Token::Type::Operator:
        {
            const auto rhs = stack.back();
            stack.pop_back();
            const auto lhs = stack.back();
            stack.pop_back();

            switch (token.str[0]) {
            default:
                std::cout << "Operator error [" <<  token.str.c_str() << "]\n";
                exit(0);
                break;
            case '^':
                stack.push_back(static_cast<double>(pow(lhs, rhs)));
                break;
            case '*':
                stack.push_back(lhs * rhs);
                break;
            case '/':
                stack.push_back(lhs / rhs);
                break;
            case '+':
                stack.push_back(lhs + rhs);
                break;
            case '-':
                stack.push_back(lhs - rhs);
                break;
            }
            op = "Push " + std::to_string(lhs) + " " + token.str + " " + std::to_string(rhs);
        }
        break;

        default:
            std::cout << "Token error\n";
            exit(0);
        }
#ifdef DEBUG
        debugOutput(token, queue, stack, op);
#endif // DEBUG
    }

    std::cout << "result = " << stack.back() << std::endl;
}
