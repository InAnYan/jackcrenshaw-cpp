#include <cstdio>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <fstream>

char look;
std::unordered_map<std::string, int> table;
std::ifstream file;

void GetChar()
{
    file.read(&look, 1);

    if (file.eof())
    {
        look = '\0';
    }
}

void Error(const std::string& msg)
{
    std::cerr << "Error: " << msg << std::endl;
}

void Abort(const std::string& msg)
{
    Error(msg);
    exit(1);
}

void Expected(const std::string& msg)
{
    Abort(msg + " expected");
}

bool IsWhite(char ch)
{
    switch (ch)
    {
    case ' ':
    case '\r':
    case '\t':
        return true;
    default:
        return false;
    }
}

void SkipWhite()
{
    while (IsWhite(look))
    {
        GetChar();
    }
}

void NewLine()
{
    if (look == '\r')
    {
        GetChar();

        if (look == '\n')
        {
            GetChar();
        }
    }
    else if (look == '\n')
    {
        GetChar();
    }
}

void Match(char ch)
{
    if (look == ch)
    {
        GetChar();
        SkipWhite();
    }
    else
    {
        Expected("'" + std::string(1, ch) + "'");
    }
}

char UpCase(char ch)
{
    if ((ch >= 'a' && ch <= 'z'))
    {
        return ch - 'a' + 'A';
    }

    return ch;
}

std::string UpCase(const std::string& str)
{
    std::string res(str);

    std::transform(res.begin(), res.end(), res.begin(), [](char ch) { return tolower(ch); });

    return res;
}

bool IsAlpha(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool IsDigit(char ch)
{
    return ch >= '0' && ch <= '9';
}

bool IsAlnum(char ch)
{
    return IsAlpha(ch) || IsDigit(ch);
}

std::string GetName()
{
    if (!IsAlpha(look))
    {
        Expected("name");
    }
    
    std::string token("");
    
    do
    {
        token += UpCase(look);
        GetChar();
    } while (IsAlnum(look));
        
    SkipWhite();

    return token;
}

int GetNum()
{
    if (!IsDigit(look))
    {
        Expected("number");
    }
    
    int num = 0;
    
    while (IsDigit(look))
    {
        num = num * 10 + (look - '0');
        GetChar();
    }

    SkipWhite();

    return num;
}

void EmitLn(const std::string& str)
{
    std::cout << std::string(4, ' ') << str << std::endl;
}

void Init(const char* path)
{
    file.open(path);
    if (!file)
    {
        Abort("unable to open file");
    }

    GetChar();
    SkipWhite();
}

bool IsAddop(char ch)
{
    return ch == '+' || ch == '-';
}

void Input()
{
    Match('?');
    std::cin >> table[GetName()];
    
}

void Output()
{
    Match('!');
    std::cout << table[GetName()] << std::endl;
}

int Expression();

int Factor()
{
    int res;
    
    if (look == '(')
    {
        Match('(');
        res = Expression();
        Match(')');
    }
    else if (IsAlpha(look))
    {
        res = table[GetName()];
    }
    else
    {
        res = GetNum();
    }

    return res;
}

int Term()
{
    int value = Factor();

    while (look == '*' || look == '/')
    {
        switch (look)
        {
        case '*':
            Match('*');
            value = value * Factor();
            break;
        case '/':
            Match('/');
            value = value / Factor();
            break;
        default:
            break;
        }
    }

    return value;
}

int Expression()
{
    int value;

    if (IsAddop(look))
    {
        value = 0;
    }
    else
    {
        value = Term();
    }

    while (IsAddop(look))
    {
        switch (look)
        {
        case '+':
            Match('+');
            value = value + Term();
            break;
        case '-':
            Match('-');
            value = value - Term();
            break;
        default:
            break;
        }
    }

    return value;
}

void Assignment()
{
    std::string name = GetName();
    Match('=');
    table[name] = Expression();
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Abort("wrong arguments count. Usage: a.out src");
        return 1;
    }

    Init(argv[1]);

    do
    {
        switch (look)
        {
        case '?': Input();     break;
        case '!': Output();    break;
        default: Assignment(); break;
        }

        NewLine();
    } while (look != '.');

    std::cout << table["B"] << std::endl;

    return 0;
}
