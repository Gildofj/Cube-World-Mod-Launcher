#include "framework/test_framework.h"

int main(int argc, char** argv) {
    return ::TestFramework::TestRegistry::instance().run_all();
}
