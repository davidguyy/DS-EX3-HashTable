// by 11227205 資訊二乙 劉至嘉 & 11027214 楊碕萍.
#include <string.h>

#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <expected>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

// anonymous namespace to force internal linkage
// I would like to move these into a header file if possible
namespace {
// inspired (copied) from absl::StatusCode
// go to https://abseil.io/docs/cpp/guides/status-codes for documentation
enum struct StatusCode : int {
  kOk = 0,
  kCancelled = 1,
  kUnknown = 2,
  kInvalidArgument = 3,
  kDeadlineExceeded = 4,
  kNotFound = 5,
  kAlreadyExists = 6,
  kPermissionDenied = 7,
  kResourceExhausted = 8,
  kFailedPrecondition = 9,
  kAborted = 10,
  kOutOfRange = 11,
  kUnimplemented = 12,
  kInternal = 13,
  kUnavailable = 14,
  kDataLoss = 15,
  kUnauthenticated = 16,
};

constexpr std::string_view kPrompt =
    "******* Hash Table *****\n"
    "* 0. QUIT              *\n"
    "* 1. Linear probing   *\n"
    "* 2. Double hashing    *\n"
    "************************\n"
    "Input a choice(0, 1, 2): ";

constexpr std::string_view kInputPrefix = "input";
constexpr std::string_view kInputSuffix = ".txt";
constexpr std::string_view kCancelString = "input0.txt";

constexpr std::vector<std::string_view> StrSplit(std::string_view string,
                                                 std::string_view delimiter) {
  auto tokens = string | std::ranges::views::split(delimiter);
  return std::vector<std::string_view>{tokens.begin(), tokens.end()};
}

constexpr std::string_view StrSplitAt(std::string_view line,
                                      std::string_view delimiter,
                                      const int at) {
  assert(at >= 0);
  auto tokens = line | std::views::split(delimiter);
  return std::string_view{*std::ranges::next(tokens.begin(), at)};
}

template <typename T>
constexpr T StrTo(std::string_view str) noexcept {
  T value = 0;
  std::from_chars(str.data(), str.data() + str.size(), value);
  return value;
}

template <typename T>
std::expected<T, StatusCode> Scan(std::istream& in = std::cin) noexcept {
  T val;
  in >> val;
  if (in.fail()) [[unlikely]] {
    return std::unexpected{StatusCode::kInvalidArgument};
  }
  return val;
}

template <typename T>
std::expected<T, StatusCode> Scan(std::string_view prompt) noexcept {
  std::print("{}", prompt);
  return Scan<T>();
}

struct Info {
  Info() = default;
  explicit Info(std::string_view line)
      : scores{StrTo<uint8_t>(StrSplitAt(line, "\t", 2)),
               StrTo<uint8_t>(StrSplitAt(line, "\t", 3)),
               StrTo<uint8_t>(StrSplitAt(line, "\t", 4)),
               StrTo<uint8_t>(StrSplitAt(line, "\t", 5)),
               StrTo<uint8_t>(StrSplitAt(line, "\t", 6)),
               StrTo<uint8_t>(StrSplitAt(line, "\t", 7))},
        score_average{StrTo<float>(StrSplitAt(line, "\t", 8))} {
    auto name = (StrSplitAt(line, "\t", 1));
    for (size_t i = 0; i < name.size(); ++i) {
      student_name[i] = name[i];
    }

    auto id = StrSplitAt(line, "\t", 0);
    for (size_t i = 0; i < name.size(); ++i) {
      student_id[i] = id[i];
    }
  }

  uint8_t scores[6]{};
  char student_name[10]{};
  char student_id[10]{};
  float score_average{};
};

std::expected<std::vector<Info>, StatusCode> MakeList(
    const std::string& file_name) {
  std::ifstream file{file_name};

  if (!file.is_open()) {
    return std::unexpected{StatusCode::kNotFound};
  }

  std::vector<Info> infos;
  std::string line;
  while (std::getline(file, line)) {
    infos.emplace_back(line);
  }

  return infos;
}

}  // namespace

int main() {
  auto input = Scan<std::string>(kPrompt);
  if (input.has_value()) {
    auto infos = MakeList(std::format("input{}.txt", input.value()));
    std::ranges::for_each(infos.value(), [](const Info& i) {
      std::println("{}\t{}\t{}", i.student_id, i.student_name, i.score_average);
    });
  }
}