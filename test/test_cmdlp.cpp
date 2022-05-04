#include "cmdlp/cmdlp.hpp"

void test_copy(cmdlp::OptionParser parser)
{
    std::cout << "Usage :\n"
              << parser.getHelp() << "\n";
    std::cout << "-d : " << parser.getOption<double>('d') << "\n";
    std::cout << "-i : " << parser.getOption<int>('i') << "\n";
    std::cout << "-s : " << parser.getOption<std::string>('s') << "\n";
    std::cout << "-v : " << parser.getOption<bool>('v') << "\n";
}

int main(int argc, char *argv[])
{
    std::vector<char *> dummy_argv;
     dummy_argv.push_back(argv[0]);
     dummy_argv.push_back((char *)"--double");
     dummy_argv.push_back((char *)"0.00006456");

     dummy_argv.push_back((char *)"--int");
     dummy_argv.push_back((char *)"42");

     dummy_argv.push_back((char *)"-s");
     dummy_argv.push_back((char *)"Hello");

     dummy_argv.push_back((char *)"--verb");

     dummy_argv.push_back((char *)"--verbose");

    cmdlp::OptionParser parser(static_cast<int>(dummy_argv.size() - 1), dummy_argv.data());
    parser.addOption('d', "--double", "Double value", 0.2);
    parser.addOption('i', "--int", "An integer value", 1);
    parser.addOption('s', "--string", "A string.. actually, a single word", "hello");
    parser.addToggle('v', "--verbose", "Enables verbose output");
    parser.parseOptions();

    test_copy(parser);

    return 0;
}
