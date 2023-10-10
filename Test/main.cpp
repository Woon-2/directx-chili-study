#include <iostream>

#include <gtest/gtest.h>

TEST(HelloTest, Hello)
{
    std::cout << "Hello, World!\n";
}

TEST(Termination, Termination)
{
    std::cout << "Press Any Key.\n";
    std::cin.get();
}