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
    
    char name = look;
    GetChar();

    return name;
}

char GetNum()
{
    if (!IsDigit(look))
    {
        Expected("number");
    }
    
    char num = look;
    GetChar();

    return num;
}

bool IsBoolean(char ch)
{
    return UpCase(ch) == 'T' || UpCase(ch) == 'F';
}

bool GetBoolean()
{
    if (!IsBoolean(look))
    {
        Expected("boolean literal");
    }

    bool res = UpCase(look) == 'T';
    GetChar();
    return res;
}

void EmitLn(const std::string& str)
{
    std::cout << std::string(4, ' ') << str << std::endl;
}

void EmitLn(char ch)
{
    std::cout << std::string(4, ' ') << ch << std::endl;
}

bool IsRelop(char ch)
{
    switch (ch)
    {
    case '=':
    case '#':
    case '<':
    case '>':
        return true;
    default:
        return false;
    }
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
        case '/': Divide();   break;
        default:              return;
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
        case '+': Add();       break;
        case '-': Substract(); break;
        default:               return;
        }
    }
}

void Equals()
{
    Match('=');
    Expression();
    EmitLn("CMP (SP)+, D0");
    EmitLn("SEQ D0");
}

void NotEquals()
{
    Match('#');
    Expression();
    EmitLn("CMP (SP)+, D0");
    EmitLn("SNE D0");
}

void Less()
{
    Match('<');
    Expression();
    EmitLn("CMP (SP)+, D0");
    EmitLn("SGE D0");
}

void Greater()
{
    Match('>');
    Expression();
    EmitLn("CMP (SP)+, D0");
    EmitLn("SLE D0");
}

void Relation()
{
    Expression();
    if (IsRelop(look))
    {
        EmitLn("MOVE D0, -(SP)");

        switch (look)
        {
        case '=': Equals();    break;
        case '#': NotEquals(); break;
        case '<': Less();      break;
        case '>': Greater();   break;
        default:               break;
        }

        EmitLn("TST D0");
    }
}

void BoolFactor()
{
    if (IsBoolean(look))
    {
        if (GetBoolean())
        {
            EmitLn("MOVE #-1, D0");
        }
        else
        {
            EmitLn("CLR D0");
        }
    }
    else
    {
        Relation();
    }
}

void NotFactor()
{
    if (look == '!')
    {
        Match('!');
        BoolFactor();
        EmitLn("EOR #-1, D0");
    }
    else
    {
        BoolFactor();
    }
}

void BoolTerm()
{
    NotFactor();

    while (look == '&')
    {
        EmitLn("MOVE D0, -(SP)");
        Match('&');
        NotFactor();
        EmitLn("AND (SP)+, D0");
    }
}

void BoolOr()
{
    Match('|');
    BoolTerm();
    EmitLn("OR (SP)+, D0");
}

void BoolXor()
{
    Match('~');
    BoolTerm();
    EmitLn("EOR (SP)+, D0");
}

bool IsOrOp(char ch)
{
    return ch == '|' || ch == '~';
}

void BoolExpression()
{
    BoolTerm();

    while (IsOrOp(look))
    {
        EmitLn("MOVE D0, -(SP)");
        switch (look)
        {
        case '|': BoolOr();  break;
        case '~': BoolXor(); break;
        default:             break;
        }
    }
}

void Other()
{
    EmitLn(GetName());
}

int labelCount = 0;

std::string NewLabel()
{
    labelCount++;
    return "L" + std::to_string(labelCount);
}

void PostLabel(const std::string& label)
{
    std::cout << label << ":" << std::endl;
}

void Block(const std::string& breakLabel);

void If(const std::string& breakLabel)
{
    Match('i');
    std::string l1 = NewLabel();
    std::string l2 = l1;
    BoolExpression();
    EmitLn("BEQ " + l1);
    Block(breakLabel);
    if (look == 'l')
    {
        Match('l');
        l2 = NewLabel();
        EmitLn("BRA " + l2);
        PostLabel(l1);
        Block(breakLabel);
    }
    Match('e');
    PostLabel(l2);
}

void While()
{
    Match('w');

    std::string l1 = NewLabel();
    std::string l2 = NewLabel();

    PostLabel(l1);
    BoolExpression();
    EmitLn("BEQ " + l2);

    Block(l2);
    EmitLn("BRA " + l1);

    PostLabel(l2);
}

void Loop()
{
    Match('p');
    std::string l1 = NewLabel();
    std::string l2 = NewLabel();

    PostLabel(l1);
    Block(l2);
    Match('e');
    EmitLn("BRA " + l1);

    PostLabel(l2);
}

void RepeatUntil()
{
    Match('r');
    std::string l1 = NewLabel();
    std::string l2 = NewLabel();

    PostLabel(l1);
    Block(l2);
    Match('u');
    BoolExpression();
    EmitLn("BEQ " + l1);

    PostLabel(l2);
}

void For()
{
    Match('f');

    std::string l1 = NewLabel();
    std::string l2 = NewLabel();

    char name = GetName();

    Match('=');
    Expression();
    EmitLn("SUBQ #1, D0");
    EmitLn("LEA " + std::string(1, name) + "(PC), A0");
    EmitLn("MOVE D0, (A0)");
    Expression();
    EmitLn("MOVE D0, -(SP)");

    PostLabel(l1);
    EmitLn("LEA " + std::string(1, name) + "(PC), A0");
    EmitLn("MOVE (A0), D0");
    EmitLn("ADDQ #1, D0");
    EmitLn("MOVE D0, (A0)");
    EmitLn("CMP (SP), D0");
    EmitLn("BLE " + l2);

    Block(l2);
    Match('e');
    EmitLn("BRA " + l1);

    PostLabel(l2);
    EmitLn("ADDQ #2, SP");
}

void Do()
{
    Match('d');
    std::string l1 = NewLabel();
    std::string l2 = NewLabel();

    Expression();
    EmitLn("SUBQ #1, D0");

    PostLabel(l1);
    EmitLn("MOVE D0, -(SP)");

    Block(l2);

    EmitLn("MOVE (SP)+, D0");
    EmitLn("DBRA D0, " + l1);
    EmitLn("SUBQ #2, (SP)");

    PostLabel(l2);
    EmitLn("ADDQ #2, (SP)");
}

void Break(const std::string& l)
{
    Match('b');

    if (l != "")
    {
        EmitLn("BRA " + l);
    }
    else
    {
        Abort("break statement appeared outside of loop");
    }
}

void Fin()
{
    if (look == '\r') GetChar();
    if (look == '\n') GetChar();
    if (look == '\r') GetChar();
}

void Block(const std::string& breakLabel)
{
    while (look != 'e' && look != 'l' && look != 'u')
    {
        Fin();
        switch (look)
        {
        case 'i': If(breakLabel);    break;
        case 'w': While();           break;
        case 'p': Loop();            break;
        case 'r': RepeatUntil();     break;
        case 'f': For();             break;
        case 'd': Do();              break;
        case 'b': Break(breakLabel); break;
        default:  Other();
        }
        Fin();
    }
}

void Program()
{
    Block("");

    if (look != 'e')
    {
        Expected("end");
    }

    EmitLn("END");
}

void Init()
{
    GetChar();
}

int main()
{
    Init();

    Program();

    return 0;
}
