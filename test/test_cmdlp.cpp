#include "cmdlp/cmdlp.hpp"

int main(int argc, char *argv[])
{
    cmdlp::OptionParser option(argc, argv);
    option.addVOption('d', "--double", "The time delta", 0.2, false);
    option.addVOption('i', "--int", "The maximum simulated time", 1, false);
    option.addVOption('s', "--string", "Used to determine when a solution is considered complete", "hello", false);
    option.addTOption('v', "--verbose", "Enables verbose output.", false);
    option.parseOptions();
    std::cout << "-d : " << option.getOption<double>('d') << "\n";
    std::cout << "-i : " << option.getOption<int>('i') << "\n";
    std::cout << "-s : " << option.getOption<std::string>('s') << "\n";
    std::cout << "-v : " << option.getOption<bool>('v') << "\n";
    std::cout << "help:\n"
              << option.getHelp() << "\n";
    return 0;
}
