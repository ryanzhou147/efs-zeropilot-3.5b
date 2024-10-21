#pragma once

#include "printer_iface.hpp"

class Printer : public IPrinter
{
public:
  Printer() = default;

  void print(char *str) override;
};
