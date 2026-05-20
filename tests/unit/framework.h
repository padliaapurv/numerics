/* Shared minimal test framework for all unit tests. */
#pragma once
#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
#include <functional>

static int g_pass = 0, g_fail = 0;

static void run(const std::string& name, std::function<bool(std::string&)> fn)
{
    std::string detail;
    bool ok = fn(detail);
    if (ok) ++g_pass; else ++g_fail;
    std::cout << (ok ? "  [PASS]" : "  [FAIL]") << "  " << name;
    if (!ok) std::cout << "\n         " << detail;
    std::cout << "\n";
}

static bool near_tol(double a, double b, double tol = 1e-12)
{
    return std::fabs(a - b) < tol;
}

static int suite_result(const char* name)
{
    std::cout << "\n" << name << "\n";
    std::cout << std::string(std::strlen(name), '=') << "\n";
    std::cout << g_pass << " passed, " << g_fail << " failed / "
              << g_pass + g_fail << " total\n";
    return (g_fail == 0) ? 0 : 1;
}
