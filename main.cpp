#include <iostream>

char look;

void GetChar()
{
    std::cin >> look;
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

void Match(char ch)
{
    if (look == ch)
    {
        GetChar();
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

bool IsAlpha(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool IsDigit(char ch)
{
    return ch >= '0' && ch <= '9';
}

char GetName()
{
    if (!IsAlpha(look))
    {
        Expected("name");
    }

    char name = UpCase(look);
    GetChar();
    return name;
}

char GetNum()
{
    if (!IsDigit(look))
    {
        Expected("name");
    }

    char name = look;
    GetChar();
    return name;
}

void EmitLn(const std::string& str)
{
    std::cout << std::string(4, ' ') << str << std::endl;
}

void Init()
{
    GetChar();
}

void Expression();

void Ident()
{
    char var = GetName();

    if (look == '(')
    {
        Match('(');
        Match(')');
        EmitLn("BSR " + std::string(1, var));
    }
    else
    {
        EmitLn("MOVE " + std::string(1, var) + "(PC), D0");
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
        char num = GetNum();
        EmitLn("MOVE #" + std::string(1, num) + ", D0");
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
    char name = GetName();
    Match('=');
    Expression();
    EmitLn("LEA " + std::string(1, name) + "(PC), A0");
    EmitLn("MOVE D0, (A0)");
}

int main()
{
    Init();
    Assignment();
    return 0;
}
