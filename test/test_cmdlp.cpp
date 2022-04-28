#include "cmdlp/cmdlp.hpp"

int main(int argc, char *argv[])
{
    std::vector<char *> __argv;
    __argv.push_back(argv[0]);
    __argv.push_back((char *)"--double");
    __argv.push_back((char *)"0.00006456");

    __argv.push_back((char *)"--int");
    __argv.push_back((char *)"42");

    __argv.push_back((char *)"-s");
    __argv.push_back((char *)"Hello");

    __argv.push_back((char *)"--verb");

    __argv.push_back((char *)"--verbose");

    cmdlp::OptionParser parser(__argv.size() - 1, __argv.data());
    parser.addOption('d', "--double", "Double value", 0.2);
    parser.addOption('i', "--int", "An integer value", 1);
    parser.addOption('s', "--string", "A string.. actually, a single word", "hello");
    parser.addToggle('v', "--verbose", "Enables verbose output");
    parser.parseOptions();
    std::cout << "Usage :\n"
              << parser.getHelp() << "\n";
    std::cout << "-d : " << parser.getOption<double>('d') << "\n";
    std::cout << "-i : " << parser.getOption<int>('i') << "\n";
    std::cout << "-s : " << parser.getOption<std::string>('s') << "\n";
    std::cout << "-v : " << parser.getOption<bool>('v') << "\n";
    return 0;
}
