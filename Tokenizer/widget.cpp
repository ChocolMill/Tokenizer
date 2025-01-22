#include "widget.h"
#include "ui_widget.h"
#include <map>
#include <QFile>

enum TokenID {
    ABSTRACT, AS, ASYNC, AWAIT, BECOME, BOX, BREAK, DO, CONST, CONTINUE, CRATE, ELSE, ENUM, EXTERN, FALSE, FINAL,
    FN, FOR, IF, IMPL, IN, LET, LOOP, MACRO, MATCH, MOD, MOVE, MUT, OVERRIDE, PRIV, PUB, REF, RETURN, sELF, SELF, STATIC, STRUCT,
    SUPER, TRAIT, TRUE, TYPE, TYPEOF, UNSAFE, USE, UNSIZED, WHERE, WHILE, VIRTUAL, YIELD, UNION, // 关键词 0~49
    PRINTLN, VEC, MACRO_RULES, ASSERT_EQ, // 宏 50~53
    STRING, CHAR, I8, I16, I32, I64, I128, ISIZE, U8, U16, U32, U64, U128, USIZE, F32, F64,// 类型名 54~69
    ID, // 标识符 70
    ASSIGN, EQ, LT, GT, LTE, GTE, NOTEQ, ADD, SUB, MUL, DIV, MODOP, POWER, NOT, AND, OR, BITAND, BITOR, BITXOR, LSHIFT, RSHIFT, ADDASS, SUBASS, MULASS, QM,
    DIVASS, MODASS, LSHIFTASS, RSHIFTASS, BITANDASS, BITORASS, BITXORASS, SCOPE1, SCOPE2, SCOPE3, DOT, ARROW1, ARROW2, // 操作符 71~108
    DECINT, DECFLOAT, BININT, BINFLOAT, OCTINT, OCTFLOAT, HEXINT, HEXFLOAT,    // 字面量 109~116 *
    STR,    // 字符串字面量 117
    COMMENT, // 注释 118
    LBRACKET, RBRACKET, LSBRACKET, RSBRACKET, LBRACE, RBRACE, SEMICOLON, COLON, COMMA, // 分隔符 119~127
    _0_, _T_, _N_, _R_, _SQUOTE_, _DQUOTE_,     // 转义字符 128~133
    BYTESTR,        // 字节字符串
    RAWSTR,         // 原始字符串
    LIFECYCLE,  // 生命周期
    QUALIFIER,  // 限定符
    _CHAR_,     // 字符
    BYTE,       // 字节
    MACROP,     // 宏操作符
    INIT, ENDINPUT, NSM
};
// NSM: 无特殊含义

// 单词结构
struct TokenStru
{
    TokenID  ID = INIT;
    double val;
    char op[5];
    char comment[1024];      // 注释符
    char word[20];
    char bodhStr[35];    // binary octal hexadecimal string
    char str[1024];
};

TokenStru token;
const char* buffer;

bool flag1 = false;          // 多行注释范围标识
bool flag2 = false;          // 单行注释范围标识
bool flag3 = false;          // 文件注释范围标识
bool flagStr = false;        // 多行字符串范围表示
QString line;
//string pathStr;
QString filePath;  // 存储文件路径

map<QString, TokenID> keywords;
map<TokenID, QString> bodh; // binary octal hexadecimal
map<QString, TokenID> className;
map<QString, TokenID> macro;
map<QString, TokenID> esc;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->resize(1400, 800);     //  设置窗口初始大小
}

Widget::~Widget()
{
    delete ui;
}

int hexCharToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + c - 'A';
    if (c >= 'a' && c <= 'f') return 10 + c - 'a';
    return 0; // 不应该发生，除非输入不是有效的十六进制字符
}

void init()
{
    keywords["abstract"] = ABSTRACT;
    keywords["as"] = AS;
    keywords["async"] = ASYNC;
    keywords["await"] = AWAIT;
    keywords["become"] = BECOME;
    keywords["box"] = BOX;
    keywords["break"] = BREAK;
    keywords["do"] = DO;
    keywords["const"] = CONST;
    keywords["continue"] = CONTINUE;
    keywords["crate"] = CRATE;
    keywords["else"] = ELSE;
    keywords["enum"] = ENUM;
    keywords["extern"] = EXTERN;
    keywords["false"] = FALSE;
    keywords["final"] = FINAL;
    keywords["fn"] = FN;
    keywords["for"] = FOR;
    keywords["if"] = IF;
    keywords["impl"] = IMPL;
    keywords["in"] = IN;
    keywords["let"] = LET;
    keywords["loop"] = LOOP;
    keywords["macro"] = MACRO;
    keywords["match"] = MATCH;
    keywords["mod"] = MOD;
    keywords["move"] = MOVE;
    keywords["mut"] = MUT;
    keywords["override"] = OVERRIDE;
    keywords["priv"] = PRIV;
    keywords["pub"] = PUB;
    keywords["ref"] = REF;
    keywords["return"] = RETURN;
    keywords["self"] = sELF;
    keywords["Self"] = SELF;
    keywords["static"] = STATIC;
    keywords["struct"] = STRUCT;
    keywords["super"] = SUPER;
    keywords["trait"] = TRAIT;
    keywords["true"] = TRUE;
    keywords["type"] = TYPE;
    keywords["typeof"] = TYPEOF;
    keywords["unsafe"] = UNSAFE;
    keywords["use"] = USE;
    keywords["unsized"] = UNSIZED;
    keywords["where"] = WHERE;
    keywords["while"] = WHILE;
    keywords["virtual"] = VIRTUAL;
    keywords["yield"] = YIELD;
    keywords["union"] = UNION;
    bodh[DECINT] = "（十进制整数）";
    bodh[DECFLOAT] = "（十进制浮点数）";
    bodh[BININT] = "（二进制整数）";
    bodh[BINFLOAT] = "（二进制浮点数）";
    bodh[OCTINT] = "（八进制整数）";
    bodh[OCTFLOAT] = "（八进制浮点数）";
    bodh[HEXINT] = "（十六进制整数）";
    bodh[HEXFLOAT] = "（十六进制浮点数）";
    className["String"] = STRING;
    className["char"] = CHAR;
    className["i8"] = I8;
    className["i16"] = I16;
    className["i32"] = I32;
    className["i64"] = I64;
    className["i128"] = I128;
    className["isize"] = ISIZE;
    className["u8"] = U8;
    className["u16"] = U16;
    className["u32"] = U32;
    className["u64"] = U64;
    className["u128"] = U128;
    className["usize"] = USIZE;
    className["f32"] = F32;
    className["f64"] = F64;
    macro["vec!"] = VEC;
    macro["println!"] = PRINTLN;
    macro["macro_rules!"] = MACRO_RULES;
    macro["assert_eq!"] = ASSERT_EQ;
    esc["\\n"] = _N_;
    esc["\\t"] = _T_;
    esc["\\r"] = _R_;
    esc["\\0"] = _0_;
    esc["\'"] = _SQUOTE_;
    esc["\""] = _DQUOTE_;
}

void Widget::GetToken()
{
    int s;

    while ((buffer[pos] == 32 || buffer[pos] == 9) && !(flag1 || flag2 || flag3))  // spacebar or tab 注释的space和tab无需跳过
        pos++;
    //cout << "pos11: " << pos << endl;
    if (flag1) {     // 多行注释
        int i = 0;
        memset(token.comment, '\0', sizeof(token.comment)); // 将数组所有元素设置为'\0'
        if (token.ID != COMMENT) {
            pos--;
        }
        //cout << "pos12: " << pos << endl;
        while (!(buffer[pos] == '*' && buffer[pos + 1] == '/') && buffer[pos] != '\0') {
            token.comment[i++] = buffer[pos];
            pos++;
            //cout << "pos13: " << pos << endl;
        }
        if (buffer[pos] == '*' && buffer[pos + 1] == '/') {
            token.comment[i++] = '*'; token.comment[i++] = '/';
            token.comment[i] = '\0';
            pos += 2;
            //cout << "pos14: " << pos << endl;
            flag1 = false;       // 多行注释结束
        }
        token.ID = COMMENT;
    }
    else if (flag2) {       // 单行注释
        pos--;
        //cout << "pos15: " << pos << endl;
        int j = 0;
        for (j = 0; buffer[pos] != '\0'; pos++, j++) {
            token.comment[j] = buffer[pos];
            //cout << "pos16: " << pos << endl;
        }
        token.comment[j] = '\0';
        flag2 = false;
        token.ID = COMMENT;
    }
    else if (flag3) {       // 文件注释
        pos--;
        int j = 0;
        for (j = 0; buffer[pos] != '\0'; pos++, j++) {
            token.comment[j] = buffer[pos];
            //cout << "pos17: " << pos << endl;
        }
        token.comment[j] = '\0';
        flag3 = false;
        token.ID = COMMENT;
    }
    else if (buffer[pos] == '"' || flagStr) {
        // string str = "Hello world";
        // "Hello world"
        token.ID = STR;
        flagStr = true;
        memset(token.str, '\0', sizeof(token.str)); // 将数组所有元素设置为'\0'
        int k = 0;
        token.str[k++] = buffer[pos++];
        //cout << "pos127: " << pos << endl;
        while (buffer[pos] != '"' && buffer[pos] != '\0') {
            token.str[k++] = buffer[pos++];
            //cout << "pos128: " << pos << endl;
        }
        if (buffer[pos] == '"') {
            token.str[k++] = buffer[pos++];
            flagStr = false;
            //cout << "pos129: " << pos << endl;
        }
        token.str[k] = '\0';
    }
    else if ((buffer[pos] >= '0') && (buffer[pos] <= '9'))  // digit
    {
        s = 0;
        if (buffer[pos] == '0' && (buffer[pos + 1] == 'b' || buffer[pos + 1] == 'B')) { // 二进制
            token.bodhStr[0] = '0'; token.bodhStr[1] = buffer[pos + 1];
            pos += 2;   // 跳过前缀
            //cout << "pos18: " << pos << endl;
            int k = 2;
            while (buffer[pos] == '0' || buffer[pos] == '1' || buffer[pos] == '_')
            {
                if (buffer[pos] == '_') {
                    token.bodhStr[k++] = buffer[pos];
                    pos++;
                    //cout << "pos19: " << pos << endl;
                    continue;
                }
                s = s * 2 + buffer[pos] - '0';
                token.bodhStr[k++] = buffer[pos];
                pos++;
                //cout << "pos110: " << pos << endl;
            }
            token.ID = BININT; token.val = s;
            if (buffer[pos] == '.'  && buffer[pos + 1] != '.')
            {
                token.ID = BINFLOAT;
                token.bodhStr[k++] = '.';
                int len = 0;
                s = 0; pos++;
                //cout << "pos111: " << pos << endl;
                while (buffer[pos] == '0' || buffer[pos] == '1')
                {
                    s = s * 2 + buffer[pos] - '0'; len++;
                    token.bodhStr[k++] = buffer[pos];
                    pos++;
                    //cout << "pos112: " << pos << endl;
                }
                token.val = token.val + s / pow(2, len);
            }
            token.bodhStr[k] = '\0';
            // k = 2;
        }
        else if (buffer[pos] == '0' && (buffer[pos + 1] == 'o' || buffer[pos + 1] == '0')) {    // 八进制
            token.bodhStr[0] = '0'; token.bodhStr[1] = buffer[pos + 1];
            pos += 2;   // 跳过前缀
            //cout << "pos113: " << pos << endl;
            int k = 2;
            while (buffer[pos] >= '0' && buffer[pos] <= '7' || buffer[pos] == '_')
            {
                if (buffer[pos] == '_') {
                    token.bodhStr[k++] = buffer[pos];
                    pos++;
                    //cout << "pos114: " << pos << endl;
                    continue;
                }
                s = s * 8 + buffer[pos] - '0';
                token.bodhStr[k++] = buffer[pos];
                pos++;
                //cout << "pos115: " << pos << endl;
            }
            token.ID = OCTINT; token.val = s;
            if (buffer[pos] == '.'  && buffer[pos + 1] != '.')
            {
                token.ID = OCTFLOAT;
                int len = 0;
                s = 0; pos++;
                //cout << "pos116: " << pos << endl;
                token.bodhStr[k++] = '.';
                while (buffer[pos] >= '0' && buffer[pos] <= '7')
                {
                    s = s * 8 + buffer[pos] - '0'; len++;
                    token.bodhStr[k++] = buffer[pos];
                    pos++;
                    //cout << "pos117: " << pos << endl;
                }
                token.val = token.val + s / pow(8, len);
            }
            token.bodhStr[k] = '\0';
        }
        else if (buffer[pos] == '0' && (buffer[pos + 1] == 'x' || buffer[pos + 1] == 'X')) {    // 十六进制
            token.bodhStr[0] = '0'; token.bodhStr[1] = buffer[pos + 1];
            pos += 2;   // 跳过前缀
            //cout << "pos118: " << pos << endl;
            int k = 2;
            while ((buffer[pos] >= '0' && buffer[pos] <= '9') || (buffer[pos] >= 'a' && buffer[pos] <= 'f') || (buffer[pos] >= 'A' && buffer[pos] <= 'F') || buffer[pos] == '_') {
                if (buffer[pos] == '_') {
                    token.bodhStr[k++] = buffer[pos];
                    pos++;
                    //cout << "pos119: " << pos << endl;
                    continue;
                }
                s = s * 16 + hexCharToInt(buffer[pos]);
                token.bodhStr[k++] = buffer[pos];
                pos++;
                //cout << "pos120: " << pos << endl;
            }
            token.ID = HEXINT; token.val = s;
            if (buffer[pos] == '.'  && buffer[pos + 1] != '.')
            {
                token.ID = HEXFLOAT;
                int len = 0;
                s = 0; pos++;
                token.bodhStr[k++] = '.';
                while ((buffer[pos] >= '0' && buffer[pos] <= '9') || (buffer[pos] >= 'a' && buffer[pos] <= 'f') || (buffer[pos] >= 'A' && buffer[pos] <= 'F'))
                {
                    s = s * 16 + buffer[pos] - '0'; len++;
                    token.bodhStr[k++] = buffer[pos];
                    pos++;
                    //cout << "pos121: " << pos << endl;
                }
                token.val = token.val + s / pow(16, len);
            }
            token.bodhStr[k] = '\0';
        }
        else {      // 十进制
            int k = 0;
            while (buffer[pos] >= '0' && buffer[pos] <= '9' || buffer[pos] == '_')
            {
                if (buffer[pos] == '_') {
                    token.bodhStr[k++] = buffer[pos];
                    pos++;
                    //cout << "pos122: " << pos << endl;
                    continue;
                }
                s = s * 10 + buffer[pos] - '0';
                token.bodhStr[k++] = buffer[pos];
                pos++;
                //cout << "pos123: " << pos << endl;
            }
            token.ID = DECINT; token.val = s;
            if (buffer[pos] == '.'  && buffer[pos + 1] != '.')
            {
                token.ID = DECFLOAT;
                int len = 0;
                s = 0; pos++;
                //cout << "pos124: " << pos << endl;
                token.bodhStr[k++] = '.';
                while (buffer[pos] >= '0' && buffer[pos] <= '9')
                {
                    s = s * 10 + buffer[pos] - '0'; len++;
                    token.bodhStr[k++] = buffer[pos];
                    pos++;
                    //cout << "pos125: " << pos << endl;
                }
                token.val = token.val + s / pow(10, len);
            }
            token.bodhStr[k] = '\0';
        }
    }
    else if (buffer[pos] == 'b' && buffer[pos + 1] == '\"') {
        token.ID = BYTESTR;
        int k = 0;
        token.str[k++] = buffer[pos++];     // 接收b
        token.str[k++] = buffer[pos++];     // 接收"
        //cout << "pos127: " << pos << endl;
        while (buffer[pos] != '"') {
            token.str[k++] = buffer[pos++];
            //cout << "pos128: " << pos << endl;
        }
        token.str[k++] = buffer[pos++];
        //cout << "pos129: " << pos << endl;
        token.str[k] = '\0';
    }
    else if (buffer[pos] == 'r' && buffer[pos + 1] == '#' && buffer[pos + 2] == '"') {
        // string str = "Hello world";
        // "Hello world"
        token.ID = RAWSTR;
        int k = 0;
        token.str[k++] = buffer[pos++];     // 接收r
        token.str[k++] = buffer[pos++];     // 接收#
        token.str[k++] = buffer[pos++];     // 接收"
        //cout << "pos127: " << pos << endl;
        while (buffer[pos] != '"' && buffer[pos + 1] != '#') {
            token.str[k++] = buffer[pos++];
            //cout << "pos128: " << pos << endl;
        }
        token.str[k++] = buffer[pos++];
        token.str[k++] = buffer[pos++];
        //cout << "pos129: " << pos << endl;
        token.str[k] = '\0';
    }
    else if (buffer[pos] == 'b' && buffer[pos + 1] == '\'') {
        token.ID = BYTE;
        token.word[0] = buffer[pos++];      // 接收b
        token.word[1] = buffer[pos++];      // 接收'
        token.word[2] = buffer[pos++];      // 接收字符
        token.word[3] = buffer[pos++];      // 接收'
        token.word[4] = '\0';
    }
    else if (isalpha(buffer[pos]) || buffer[pos] == '_')
    {
        int len = 0;
        while (isalpha(buffer[pos]) || isdigit(buffer[pos]) || buffer[pos] == '_' || buffer[pos] == '!')
        {
            token.word[len] = buffer[pos];
            len++;
            pos++;
            //cout << "pos126: " << pos << endl;
        }
        token.word[len] = '\0';
        //cout << "(getToken: " << token.word << ")" << endl;
        map<QString, TokenID>::iterator it1;
        map<QString, TokenID>::iterator it2;
        map<QString, TokenID>::iterator it3;
        it1 = keywords.find(token.word);    // 关键词
        it2 = macro.find(token.word);    // 宏
        it3 = className.find(token.word);    // 类型名
        if (it1 != keywords.end())
            token.ID = keywords[token.word];
        else if (it2 != macro.end())
            token.ID = macro[token.word];
        else if (it3 != className.end())
            token.ID = className[token.word];
        else token.ID = ID;
    }
    else if (buffer[pos] == '\'' && isalpha(buffer[pos + 1]) && (isdigit(buffer[pos + 2]) || isspace(buffer[pos + 2]) ||  buffer[pos + 2] == ':' || buffer[pos + 2] == '>')) {   // 生命周期
        int k = 0;
        token.ID = LIFECYCLE;
        while (!(isspace(buffer[pos]) || buffer[pos] == ':' || buffer[pos] == '>')) {
            token.word[k++] = buffer[pos++];
        }
        token.word[k] = '\0';
    }
    else if (buffer[pos] == '\'' && buffer[pos + 2] == '\'') {
        token.ID = _CHAR_;
        token.word[0] = buffer[pos++];
        token.word[1] = buffer[pos++];
        token.word[2] = buffer[pos++];
        token.word[3] = '\0';
    }
    else if (buffer[pos] != '\0')
    {
        switch (buffer[pos])
        {
        case '+': token.op[0] = '+';
            if (buffer[pos + 1] == '=') {       // 加法赋值
                pos++; token.ID = ADDASS; token.op[1] = '='; token.op[2] = '\0';
            }
            else {
                token.ID = ADD; token.op[1] = '\0';  // 加法
            }
            break;
        case '-': token.op[0] = '-';
            if (buffer[pos + 1] == '=') {       // 减法赋值
                pos++; token.ID = SUBASS; token.op[1] = '='; token.op[2] = '\0';
            }
            else if (buffer[pos + 1] == '>') {       // ->
                pos++; token.ID = ARROW1; token.op[1] = '>'; token.op[2] = '\0';
            }
            else {
                token.ID = SUB; token.op[1] = '\0';  // 减法
            }
            break;
        case '*': token.op[0] = '*';
            if (buffer[pos + 1] == '*') {   // 幂运算
                pos++; token.ID = POWER; token.op[1] = '*'; token.op[2] = '\0';
            }
            else if (buffer[pos + 1] == '=') {       // 乘法赋值
                pos++; token.ID = MULASS; token.op[1] = '='; token.op[2] = '\0';
            }
            else {
                token.ID = MUL; token.op[1] = '\0'; // 乘法
            }
            break;
        case '/':
            if (buffer[pos + 1] == '*') {       // 多行注释起始符
                flag1 = true;        // 多行注释开始
                token.ID = NSM;
            }
            else if (buffer[pos + 1] == '/' && buffer[pos + 2] == '/') {        // 文件注释符
                flag2 = true;
                token.ID = NSM;
            }
            else if (buffer[pos + 1] == '/') {      // 单行注释符
                flag2 = true;
                token.ID = NSM;
            }
            else if (buffer[pos + 1] == '=') {       // 除法赋值
                pos++; token.ID = DIVASS; token.op[1] = '='; token.op[2] = '\0';
            }
            else {
                token.ID = DIV; token.op[0] = buffer[pos]; token.op[1] = '\0';      // 除法
            }
            break;
        case '%': token.op[0] = '%';
            if (buffer[pos + 1] == '=') {       // 取模赋值
                pos++; token.ID = MODASS; token.op[1] = '='; token.op[2] = '\0';
            }
            else {
                token.ID = MODOP; token.op[1] = '\0';  // 取模
            }
            break;
        case '(': token.ID = LBRACKET; token.op[0] = buffer[pos]; token.op[1] = '\0'; break;
        case ')': token.ID = RBRACKET; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case '[': token.ID = LSBRACKET; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case ']': token.ID = RSBRACKET; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case '{': token.ID = LBRACE; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case '}': token.ID = RBRACE; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case ';': token.ID = SEMICOLON; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case ':': token.ID = COLON; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case ',': token.ID = COMMA; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case '@': token.ID = MACROP; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case '#': token.ID = MACROP; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case '$': token.ID = MACROP; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case '\\':
        {
            token.word[0] = buffer[pos++];
            token.word[1] = buffer[pos];
            token.word[2] = '\0';
            map<QString, TokenID>::iterator it;
            it = esc.find(token.word);    // 关键词
            if (it != esc.end())
                token.ID = esc[token.word];
            break;
        }
        case '?': token.ID = QM; token.op[0] = buffer[pos]; token.op[1] = '\0';  break;
        case '.': token.op[0] = '.';
            if (buffer[pos + 1] == '.' && buffer[pos + 2] == '.') {     // ...
                pos += 2; token.ID = SCOPE3; token.op[1] = '.'; token.op[2] = '.';  token.op[3] = '\0';
            }
            else if (buffer[pos + 1] == '.' && buffer[pos + 2] == '=') {        // ..=
                pos += 2; token.ID = SCOPE2; token.op[1] = '.'; token.op[2] = '=';  token.op[3] = '\0';
            }
            else if (buffer[pos + 1] == '.') {      // ..
                pos++; token.ID = SCOPE1; token.op[1] = '.'; token.op[2] = '\0';
            }
            else {      // .
                token.ID = DOT; token.op[0] = buffer[pos]; token.op[1] = '\0';
            }
            break;
        case '>': token.op[0] = '>';
            if (buffer[pos + 1] == '=')     // 大于等于
            {
                pos++; token.ID = GTE; token.op[1] = '='; token.op[2] = '\0';
            }
            else if (buffer[pos + 1] == '>' && buffer[pos + 2] == '=') {        // 右移赋值
                pos += 2; token.ID = RSHIFTASS; token.op[1] = '>'; token.op[2] = '=';  token.op[3] = '\0';
            }
            else if (buffer[pos + 1] == '>') {      // 右移
                pos++; token.ID = RSHIFT; token.op[1] = '>'; token.op[2] = '\0';
            }
            else {
                token.op[1] = '\0'; token.ID = GT; // 大于
            }
            break;
        case '<': token.op[0] = '<';
            if (buffer[pos + 1] == '=')     // 小于等于
            {
                pos++; token.ID = LTE; token.op[1] = '='; token.op[2] = '\0';
            }
            else if (buffer[pos + 1] == '<' && buffer[pos + 2] == '=') {        // 左移赋值
                pos += 2; token.ID = LSHIFTASS; token.op[1] = '<'; token.op[2] = '=';  token.op[3] = '\0';
            }
            else if (buffer[pos + 1] == '<') {      // 左移
                pos++; token.ID = LSHIFT; token.op[1] = '<'; token.op[2] = '\0';
            }
            else {
                token.op[1] = '\0'; token.ID = LT;      // 小于
            }
            break;
        case '=': token.op[0] = '=';
            if (buffer[pos + 1] == '=')     // 等于
            {
                pos++; token.ID = EQ; token.op[1] = '='; token.op[2] = '\0';
                //cout << "pos132: " << pos << endl;
            }
            else if (buffer[pos + 1] == '>')     // =>
            {
                pos++; token.ID = ARROW2; token.op[1] = '>'; token.op[2] = '\0';
            }
            else {
                token.op[1] = '\0'; token.ID = ASSIGN;      // 赋值
            }
            break;
        case '!': token.op[0] = '!';
            if (buffer[pos + 1] == '=')     // 不等于
            {
                pos++; token.ID = NOTEQ; token.op[1] = '='; token.op[2] = '\0';
            }
            else {
                token.op[1] = '\0'; token.ID = NOT;   // 非
            }
            break;
        case '&': token.op[0] = '&';
            if (buffer[pos + 1] == '&') {
                pos++; token.ID = AND; token.op[1] = '&'; token.op[2] = '\0';
            }
            else if (buffer[pos + 1] == '=') {
                pos++; token.ID = BITANDASS; token.op[1] = '='; token.op[2] = '\0';
            }
            else {
                token.op[1] = '\0'; token.ID = BITAND;
            }
            break;
        case '|': token.op[0] = '|';
            if (buffer[pos + 1] == '=') {       // 位或赋值
                pos++; token.ID = BITORASS; token.op[1] = '='; token.op[2] = '\0';
            }
            else if (buffer[pos + 1] == '|') {       // 逻辑或
                pos++; token.ID = OR; token.op[1] = '|'; token.op[2] = '\0';
            }
            else {
                token.op[1] = '\0'; token.ID = BITOR; // 位或
            }
            break;
        case '^': token.op[0] = '^';
            if (buffer[pos + 1] == '=') {       // 位异或赋值
                pos++; token.ID = BITXORASS; token.op[1] = '='; token.op[2] = '\0';
            }
            else {
                token.ID = BITXOR; token.op[1] = '\0';    // 位异或
            }
            break;
        default:  cout << " Error Input at: " << pos + 1 << endl; exit(1);
        }
        pos++;
    }
    else  token.ID = ENDINPUT;

}


void Widget::getTokenMain() {
    init();
    // cout << "The result is : " << endl;
    // 改为使用QTextEdit输出


    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件：" << filePath;
        QMessageBox::warning(this, tr("打开文件失败"), tr("无法打开文件：\n%1").arg(filePath));
        return;
    }
    appendToTextEdit("The result is : \n");
    QTextStream in(&file);
    in.setCodec("UTF-8");  // 设置编码为UTF-8
    while (!in.atEnd()) {  // 循环直到文件结束
        QString line = in.readLine();  // 读取一行
        QByteArray byteData = line.toLocal8Bit();  // 将QString转换为QByteArray
        buffer = byteData.data();  // 获取C风格的字符数组

        pos = 0;
        // cout << buffer << endl;
        // 改为使用QTextEdit输出
        appendToTextEdit(QString::fromLocal8Bit(buffer));
        GetToken(); // 获取第一个单词
        while (token.ID != ENDINPUT) {
            if (token.ID <= 49) appendToTextEdit(QString(token.word) + "：关键词");
            else if (token.ID >= 119 && token.ID <= 127) appendToTextEdit(QString(token.op) + "：分隔符");
            else if (token.ID == ID) appendToTextEdit(QString(token.word) + "：标识符");
            else if (token.ID >= 71 && token.ID <= 108) appendToTextEdit(QString(token.op) + "：操作符");
            else if (token.ID >= 109 && token.ID <= 116) appendToTextEdit(QString(token.bodhStr) + "：字面量 " + bodh[token.ID]);
            else if (token.ID == STR) {
                appendToTextEdit(QString(QString::fromLocal8Bit(token.str)) + "：字符串字面量");
                if (!flagStr) token.ID = INIT;
                else break;
            }
            else if (token.ID == _CHAR_) appendToTextEdit(QString(QString::fromLocal8Bit(token.word)) + "：字符字面量");
            else if (token.ID >= 50 && token.ID <= 53) appendToTextEdit(QString(token.word) + "：宏调用名");
            else if (token.ID == MACROP) appendToTextEdit(QString(token.op) + "：宏操作符号 ");
            else if (token.ID == COMMENT)  {
                appendToTextEdit(QString(QString::fromLocal8Bit(token.comment)) + "：注释");
                if (!flag1) token.ID = INIT;
                else break;
            }
            else if (token.ID >= 54 && token.ID <= 69) appendToTextEdit(QString(token.word) + "：类型名");
            else if (token.ID >= 128 && token.ID <= 133) appendToTextEdit(QString(token.word) + "：转义字符字面量");
            else if (token.ID == LIFECYCLE) appendToTextEdit(QString(token.word) + "：生命周期");
            else if (token.ID == BYTESTR) appendToTextEdit(QString(QString::fromLocal8Bit(token.str)) + "：字节字符串字面量");
            else if (token.ID == RAWSTR) appendToTextEdit(QString(QString::fromLocal8Bit(token.str)) + "：原始字符串字面量");
            else if (token.ID == BYTE) appendToTextEdit(QString(QString::fromLocal8Bit(token.word)) + "：字节字面量");
            GetToken(); // 获取下一个单词
        }
        // cout << endl;
         appendToTextEdit("\n");
    }
    file.close();
}

void Widget::on_selectButton_clicked()
{
    // 创建一个QFileDialog对象
    QFileDialog dialog(this);

    // 设置对话框的标题
    dialog.setWindowTitle(tr("选择一个TXT文件"));

    // 设置默认的文件过滤器，只显示txt文件
    dialog.setNameFilter(tr("Text Files (*.txt)"));

    // 设置对话框的选项
    dialog.setOptions(QFileDialog::DontUseNativeDialog);

    // 显示对话框，并获取用户的选择
    if (dialog.exec())
    {
        // 获取用户选择的文件路径
        QStringList selectedFiles = dialog.selectedFiles();

        if (!selectedFiles.isEmpty())
        {
            filePath = selectedFiles.at(0);
            qDebug() << "selected file path: " << filePath;

        }
    }
}

void Widget::appendToTextEdit(const QString &text) {
    ui->resultText->append(text);  // 将运行结果添加到QTextEdit控件
}

void Widget::on_runButton_clicked()
{
        getTokenMain();
}
