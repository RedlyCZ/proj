#include <iostream>
#include <string>
#include "userInterface.hpp"

int main() {
    CLIManager cli;
    std::string inputLine;

    std::cout << "====================================================\n";
    std::cout << "         Financial Terminal CLI Initialized         \n";
    std::cout << "====================================================\n";
    std::cout << "Type commands to manage your portfolio, or 'exit' to quit.\n\n";

    // main exec loop
    while (!cli.is_failed()) {
        std::cout << "> "; //visual prompt indicator

        if (!std::getline(std::cin, inputLine)) {
            break;
        }

        if (inputLine.empty()) {
            continue;
        }

        cli.interpretInputRow(inputLine);
    }

    std::cout << "Exiting terminal. Goodbye!\n";

    return 0;
}