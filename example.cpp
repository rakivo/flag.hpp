#include <iostream>

#define FLAG_IMPLEMENTATION
#include "flag.hpp"

Flag float_flag       = Flag<double>("-f", "--float", "Floating point value");
Flag include_flag     = Flag<std::string>("-I", "--include", "Path to include files");
Flag mandatory_flag   = Flag<const char *>("-o", "--output", "Path to output file", true);
Flag always_make_flag = Flag<void>("-B", "--always-make");

void usage(void)
{
    std::cout << "Usage: ./example [options...]" << std::endl;
    std::cout << "  " << float_flag       << std::endl;
    std::cout << "  " << include_flag     << std::endl;
    std::cout << "  " << mandatory_flag   << std::endl;
    std::cout << "  " << always_make_flag << std::endl;
    std::cout << std::endl;
}

int main(int argc, const char *argv[])
{
    usage();

    Parser parser = Parser(argc, argv);

    // Check if flag is passed:
    if (parser.passed(always_make_flag))
        std::cout << "Flag is passed" << std::endl;
    else
        std::cout << "Flag is not passed" << std::endl;

    // Parse flag and get optional value
    const auto optional_value = parser.parse(include_flag);
    if (optional_value)
        std::cout << "Value: " << *optional_value << std::endl;
    else
        std::cout << "Value: NULL" << std::endl;

    // Parse flag and always get value, because we provide the default one
    const auto fvalue = parser.parse(float_flag, 69.420);
    std::cout << "Value: " << fvalue << std::endl;

    // Mandatory flag
    const auto mvalue = parser.parse(mandatory_flag);
    std::cout << "Value: " << *mvalue << std::endl;

    return 0;
}
