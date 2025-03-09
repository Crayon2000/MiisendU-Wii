#include <memory>
#if defined(HW_RVL)
    #include "application_wii.h"
    using ApplicationType = ApplicationWii;
#elif defined(HW_DOL)
    #include "application_gc.h"
    using ApplicationType = ApplicationGc;
#else
    static_assert(false, "Unsupported platform!");
#endif

/**
 * Entry point.
 * @param argc An integer that contains the count of arguments.
 * @param argv An array of null-terminated strings representing command-line arguments.
 */
int main(int argc, char *argv[]) {
    auto app = std::make_unique<ApplicationType>();
    if(argc > 0 && argv[0] != nullptr) {
        app->SetPath(argv[0]);
    }
    while(app->Run()) {}
    std::exit(0); // Exit the program
}
