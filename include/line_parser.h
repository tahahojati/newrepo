#ifndef LINE_PARSER
#define LINE_PARSER
#include <string>
#include <istream>
#include <vector>
#include <algorithm>

using std::string;
using std::istream;
using std::vector;

/**
 * This class takes a line (or input stream) and parses it using the separator token. 
 * Ex1:
 *   const string str1 = "key: value1 value2";
 *   const string str2 = "key value1 value2";
 *   LineParser lp; 
 *   lp.setModeSeparator(':');
 *   lp.readLine(str);
 *   lp.getKey()                     //key
 *   lp.getValueAt(0)                //value1
 *   lp.setModeFirstToken()
 *   lp.readLine(str2)
 *   lp.getKey()                    //key
 *   lp.setModeValuesOnly()
 *   lp.readLine(str1)
 *   lp.getValueAt(0)             // key:
 * 
 * I hope this example is self-explanatory!
 */
class LineParser {   
    public:
    bool readLine(const string&);
    bool readLine(istream&);
    string getKey();
    int getValueCount();
    string getValueAt(unsigned int);
    void setModeValuesOnly();
    void setModeFirstToken();
    void setModeSeparator(const char&);
    void setWordSeparatorToken(const char&);

    private:
    enum class Mode {
        SEPARATOR = 1,
        FIRST_TOKEN,
        VALUES_ONLY
    };
    string _str;
    char _separatorToken = 0, _wordSeparatorToken = ' '; //wordSeparator will be used to separate values. separator separates key from values.
    Mode _mode = Mode::VALUES_ONLY;
    vector<string> _tokens;
     
    bool _interpretInput();
    static inline string _trim(const string& s) {
        string res{s};
        res.erase(res.begin(), std::find_if(res.begin(), res.end(), [](int ch) {
            return !std::isspace(ch);
        })); //remove space from beginning
        res.erase(std::find_if(res.rbegin(), res.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), res.end()); //remove space from the end
        return res;
    }
};

#endif