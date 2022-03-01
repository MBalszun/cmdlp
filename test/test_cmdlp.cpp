#include "cmdlp/cmdlp.hpp"

int main(int argc, char *argv[])
{
    cmdlp::OptionParser option(argc, argv);
    option.addOption('d', "--double", "The time delta", 0.2);
    option.addOption('i', "--int", "The maximum simulated time", 1);
    option.addOption('s', "--string", "Used to determine when a solution is considered complete", "hello");
    option.parseOptions();
    std::cout << option.getOption<double>('d') << "\n";
    std::cout << option.getOption<int>('i') << "\n";
    std::cout << option.getOption<std::string>('s') << "\n";
    return 0;
}
