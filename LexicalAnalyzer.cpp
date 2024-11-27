#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_set>
#include <cctype>  // ��� isdigit, isalpha

using namespace std;

// Reserved words in C language
unordered_set<string> reservedWords = {
    "int", "float", "double", "char", "void", "if", "else", "for", "while", 
    "do", "return", "break", "continue", "switch", "case", "default", "struct", 
    "union", "typedef", "const", "unsigned", "signed", "static", "extern"
};

// Operators in C language
unordered_set<string> operators = {
    "+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||", 
    "!", "&", "|", "^", "~", "<<", ">>", "++", "--"
};

// Delimiters
unordered_set<char> delimiters = {';', ',', '(', ')', '{', '}', '[', ']', '.', '#', '?'};

// Check if a word is a reserved word
bool isReservedWord(const string &word) {
    return reservedWords.find(word) != reservedWords.end();
}

// Check if a lexeme is an operator
bool isOperator(const string &lexeme) {
    return operators.find(lexeme) != operators.end();
}

// Check if a lexeme is a delimiter
bool isDelimiter(char ch) {
    return delimiters.find(ch) != delimiters.end();
}

// Safe check for alphabetic characters
bool isSafeAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

// Safe check for alphanumeric characters
bool isSafeAlnum(char c) {
    return isSafeAlpha(c) || (c >= '0' && c <= '9');
}

// Lexical analyzer function
void lexicalAnalyzer(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    int lineNumber = 0;

    // Regular expressions for lexemes
    regex numberPattern(R"(\b(0x[0-9A-Fa-f]+|\d+\.\d+|\d+)\b)");
    regex stringPattern(R"("[^"]*")");
    regex charPattern(R"('[^']*')");
    regex preprocessorPattern(R"(#\w+)");
    regex commentPattern(R"(//.*|/\*[\s\S]*?\*/)");
    regex identifierPattern(R"(\b[a-zA-Z_]\w*\b)");

    while (getline(file, line)) {
        lineNumber++;
        smatch match;

        // Match and print comments
        while (regex_search(line, match, commentPattern)) {
            cout << "<" << match.str() << ", comment>" << endl;
            line = match.suffix().str();
        }

        // Match and print preprocessor directives
        while (regex_search(line, match, preprocessorPattern)) {
            cout << "<" << match.str() << ", preprocessor directive>" << endl;
            line = match.suffix().str();
        }

        // Match and print string literals
        while (regex_search(line, match, stringPattern)) {
            cout << "<" << match.str() << ", string literal>" << endl;
            line = match.suffix().str();
        }

        // Match and print character literals
        while (regex_search(line, match, charPattern)) {
            cout << "<" << match.str() << ", character literal>" << endl;
            line = match.suffix().str();
        }

        // Match and print numbers
        while (regex_search(line, match, numberPattern)) {
            cout << "<" << match.str() << ", number>" << endl;
            line = match.suffix().str();
        }

        // Process remaining lexemes in the line
        for (size_t i = 0; i < line.size(); ++i) {
            if (isspace(line[i])) continue;  // Skip whitespace

            // Delimiters
            if (isDelimiter(line[i])) {
                cout << "<" << line[i] << ", delimiter>" << endl;
                continue;
            }

            // Operators
            string op(1, line[i]);
            if (i + 1 < line.size() && isOperator(op + line[i + 1])) {
                op += line[++i];
            }
            if (isOperator(op)) {
                cout << "<" << op << ", operator>" << endl;
                continue;
            }

            // Identifiers and reserved words
            string lexeme;
            if (isSafeAlpha(line[i])) {
                while (i < line.size() && isSafeAlnum(line[i])) {
                    lexeme += line[i++];
                }
                --i;
                if (isReservedWord(lexeme)) {
                    cout << "<" << lexeme << ", reserved word>" << endl;
                } else {
                    cout << "<" << lexeme << ", identifier>" << endl;
                }
                continue;
            }

            // Error for unrecognized symbols
            cerr << "Unrecognized symbol at line " << lineNumber << ": '" << line[i] << "'" << endl;
        }
    }

    file.close();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Please specify a filename for lexical analysis." << endl;
        return 1;
    }

    lexicalAnalyzer(argv[1]);

    return 0;
}
