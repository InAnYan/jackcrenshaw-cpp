#include <iostream>

char look;

void GetChar()
{
    std::cin >> look;
}

void Error(const std::string& msg)
{
    std::cout << "Error: " << msg << std::endl;
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
        Expected("'" + std::string(ch, 1) + "'");
    }
}

bool IsAlpha(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

