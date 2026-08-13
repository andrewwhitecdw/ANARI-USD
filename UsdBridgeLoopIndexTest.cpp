// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <regex>
#include <string>

TEST(UsdBridgeLoopIndex, ChangeMaterialInputAttributesUsesSizeT)
{
  namespace fs = std::filesystem;

  fs::path srcPath;
#ifdef USD_BRIDGE_SOURCE_DIR
  srcPath = fs::path(USD_BRIDGE_SOURCE_DIR) / "UsdBridge" / "UsdBridge.cpp";
#else
  // Assumes this test lives in Tests/ at the repository root.
  srcPath = fs::path(__FILE__).parent_path().parent_path() / "UsdBridge" / "UsdBridge.cpp";
#endif

  std::ifstream in(srcPath);
  ASSERT_TRUE(in.is_open()) << "Could not open " << srcPath;

  std::string source((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());

  std::regex funcRegex(R"re(void\s+UsdBridge::ChangeMaterialInputAttributes\s*\([^)]*\)\s*\{)re");
  std::smatch match;
  ASSERT_TRUE(std::regex_search(source, match, funcRegex));

  // Extract the function body by counting braces.
  std::string body = source.substr(match.position() + match.length());
  std::size_t depth = 1;
  std::size_t pos = 0;
  for (; pos < body.size() && depth > 0; ++pos)
  {
    if (body[pos] == '{') ++depth;
    else if (body[pos] == '}') --depth;
  }
  body.resize(pos > 0 ? pos - 1 : 0);

  // Verify the loop that iterates over numInputAttribs uses a size_t index.
  std::regex loopRegex(R"re(\bfor\s*\(\s*size_t\s+i\s*=\s*0\s*;\s*i\s*<\s*numInputAttribs\s*;\s*\+\+i\s*\))re");
  EXPECT_TRUE(std::regex_search(body, loopRegex))
