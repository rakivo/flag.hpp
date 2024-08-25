#include <iostream>

#define FLAG_IMPLEMENTATION
#include "flag.hpp"

//////////////////////////////////////////////////////////////////////

#define print_contents(parser, flag) do { \
    if (parser.passed(flag)) \
         std::cout << "Passed: "; \
    else std::cout << "Not_passed: "; \
    const auto v = parser.parse(flag); \
    if (v) std::cout << *v << std::endl; \
    else   std::cout << "NULL" << std::endl; \
} while (0)

//////////////////////////////////////////////////////////////////////

Flag include_flag = Flag<std::string>("-I", "--include", "Path to include files");
Flag integer_flag = Flag<int>("-i", "--integer");
Flag float_flag   = Flag<float>("-f", "--float", "Floating point value");

void usage(void)
{
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << include_flag << std::endl;
    std::cout << "  " << integer_flag << std::endl;
    std::cout << "  " << float_flag << std::endl;
    std::cout << std::endl;
}

int main(int argc, const char *argv[])
{
    usage();

    Parser parser = Parser(argc, argv);

    print_contents(parser, include_flag);
    print_contents(parser, integer_flag);
    print_contents(parser, float_flag);

    return 0;
}
