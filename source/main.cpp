#include <memory>
#include "application.h"

/**
 * Entry point.
 * @param argc An integer that contains the count of arguments.
 * @param argv An array of null-terminated strings representing command-line arguments.
 */
int main(int argc, char *argv[]) {
    auto app = std::make_unique<Application>();
    if(argc > 0 && argv[0] != nullptr) {
        app->SetPath(argv[0]);
    }
    while(app->Run()) {}
    std::exit(0); // Exit the program
}
