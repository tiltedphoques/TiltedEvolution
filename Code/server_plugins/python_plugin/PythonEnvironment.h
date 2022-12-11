// Copyright (C) Vincent Hengel 2022
#pragma once

#include "Pch.h"

namespace PythonScripting
{
std::pair<std::string, std::string> split_filepath(const std::string& filepath);
bool AddPath(const char* acPath);
}
