#include <sstream>
#include <istream>
#include <algorithm>
#include <locale>
#include <vector>
#include <string>
#include "line_parser.h"

using std::getline;
using std::vector;
using std::istringstream;

void swapChars (string&, char, char);

bool LineParser::readLine(const string& str) {
    _str = str;
    return _interpretInput();
}

bool LineParser::readLine(istream& is) {
    if(is.good()) {
        getline(is, _str);
        return _interpretInput();
    }
    return false;
}

bool LineParser::_interpretInput() {
    string mystr{_trim(_str)};
    _tokens.clear();
    istringstream iss;
    string tok;

    //we swap all white space with separator! obviously we cannot differentiate between tabs and spaces.
    swapChars(mystr, ' ', _wordSeparatorToken);
    swapChars(mystr, '\t', _wordSeparatorToken);

    if(_mode == Mode::SEPARATOR) {
        string::size_type index = mystr.find_first_of(_separatorToken);
        bool not_found = index == string::npos; // the whole string will be the key. no values!
        string key = mystr.substr(0, not_found ? mystr.length() : index);
        swapChars(key, ' ', _wordSeparatorToken);
        _tokens.push_back(key); //the key
        mystr = not_found ? "" : mystr.substr(index + 1); // remove key and separator from string.
    }

    iss.str(mystr);
    while (iss.good()){
        iss >> tok;
        if(tok != "") {
            swapChars(tok, ' ', _wordSeparatorToken);
            _tokens.push_back(tok);
        }
    }
    return _tokens.size() != 0;
}

string LineParser::getKey(){
    if(_tokens.size() == 0 || _mode == Mode::VALUES_ONLY) {
        return "";
    }
    return _tokens[0];
}

int LineParser::getValueCount() {
    if(_tokens.size() == 0)
        return 0;
    return _tokens.size() - (_mode == Mode::VALUES_ONLY ? 0 : 1);
}

string LineParser::getValueAt(unsigned int i) {
    i += (_mode == Mode::VALUES_ONLY ? 0 : 1);
    if(_tokens.size()  <= i )
        return "";
    return _tokens[i];
}

void LineParser::setModeValuesOnly() {
    _mode = Mode::VALUES_ONLY;
}

void LineParser::setModeFirstToken() {
    _mode = Mode::FIRST_TOKEN;
}

void LineParser::setModeSeparator(const char& separatorToken) {
    _separatorToken = separatorToken;
    _mode = Mode::SEPARATOR;
}

void LineParser::setWordSeparatorToken(const char &ch) {
    _wordSeparatorToken = ch;
}

void swapChars(string& s, char a, char b){
    if(a == b) return;
    for (auto& ch: s) {
        if (ch == a) {
            ch = b;
        } else if (ch == b) {
            ch = a;
        }
    }
};