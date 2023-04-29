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

void EmitLn(const std::string& str)
{
    std::cout << std::string(4, ' ') << str << std::endl;
}

void EmitLn(char ch)
{
    std::cout << std::string(4, ' ') << ch << std::endl;
}

void Init()
{
    GetChar();
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

void Condition()
{
    EmitLn("<cond>");
}

void Expression()
{
    EmitLn("<expr>");
}

void If(const std::string& breakLabel)
{
    Match('i');
    std::string l1 = NewLabel();
    std::string l2 = l1;
    Condition();
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
    Condition();
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
    Condition();
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

void Block(const std::string& breakLabel)
{
    while (look != 'e' && look != 'l' && look != 'u')
    {
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

int main()
{
    Init();
    Program();

    return 0;
}
