#include <cstdio>
#include <string>
#include <iostream>
#include <algorithm>

char look;

void GetChar()
{
    look = getchar();
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

std::string GetNum()
{
    if (!IsDigit(look))
    {
        Expected("name");
    }
    
    std::string token("");
    
    while (IsDigit(look))
    {
        token += look;
        GetChar();
    }

    SkipWhite();

    return token;
}

void EmitLn(const std::string& str)
{
    std::cout << std::string(4, ' ') << str << std::endl;
}

void Init()
{
    GetChar();
    SkipWhite();
}

void Expression();

void Ident()
{
    std::string var = GetName();

    if (look == '(')
    {
        Match('(');
        Match(')');
        EmitLn("BSR " + var);
    }
    else
    {
        EmitLn("MOVE " + var + "(PC), D0");
    }
}

void Factor()
{
    if (look == '(')
    {
        Match('(');
        Expression();
        Match(')');
    }
    else if (IsAlpha(look))
    {
        Ident();
    }
    else
    {
        std::string num = GetNum();
        EmitLn("MOVE #" + num + ", D0");
    }
}

void Multiply()
{
    Match('*');
    Factor();
    EmitLn("MULS (SP)+, D0");
}

void Divide()
{
    Match('/');
    Factor();
    EmitLn("MOVE (SP)+, D1");
    EmitLn("DIVS D1, D0");
}

void Term()
{
    Factor();

    while (look == '*' || look == '/')
    {
        EmitLn("MOVE D0, -(SP)");
        
        switch (look)
        {
        case '*': Multiply(); break;
        case '/': Divide(); break;
        default: return;
        }
    }
}

void Add()
{
    Match('+');
    Term();
    EmitLn("ADD (SP)+, D0");
}

void Substract()
{
    Match('-');
    Term();
    EmitLn("SUB (SP)+, D0");
    EmitLn("NEG D0");
}

bool IsAddop(char ch)
{
    return ch == '+' || ch == '-';
}

void Expression()
{
    if (IsAddop(look))
    {
        EmitLn("CLR D0");
    }
    else
    {
        Term();
    }

    while (look == '+' || look == '-')
    {
        EmitLn("MOVE D0, -(SP)");

        switch (look)
        {
        case '+': Add(); break;
        case '-': Substract(); break;
        default: return;
        }
    }
}

void Assignment()
{
    std::string name = GetName();
    Match('=');
    Expression();
    EmitLn("LEA " + name + "(PC), A0");
    EmitLn("MOVE D0, (A0)");
}

int main()
{
    Init();

    Assignment();

    if (look != '\n')
    {
        Expected("new line");
    }

    return 0;
}
