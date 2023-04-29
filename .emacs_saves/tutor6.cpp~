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

void Init()
{
    GetChar();
}

int main()
{
    Init();
}
