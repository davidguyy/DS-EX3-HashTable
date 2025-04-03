// by 11227205 資訊二乙 劉至嘉 & 11027214 楊碕萍.
#include <algorithm>
#include <cassert>
#include <charconv>
#include <expected>
#include <format>
#include <fstream>
#include <iostream>
#include <istream>
#include <print>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <vector>

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

constexpr std::string_view kInputPrefix = "input";
constexpr std::string_view kInputSuffix = ".txt";
constexpr std::string_view kCancelString = "input0.txt";

constexpr std::string_view kPrompt = "";

namespace utils {

constexpr std::vector<std::string_view> StrSplit(std::string_view string,
                                                 std::string_view delimiter) {
  auto tokens = string | std::ranges::views::split(delimiter);
  return std::vector<std::string_view>{tokens.begin(), tokens.end()};
}

constexpr std::string_view StrSplitAt(std::string_view line,
                                      std::string_view delimiter,
                                      const int at) {
  auto tokens = line | std::views::split(delimiter);
  return std::string_view{*std::ranges::next(tokens.begin(), at)};
}

constexpr int StrToInt(std::string_view str) noexcept {
  int value = 0;
  std::from_chars(str.data(), str.data() + str.size(), value);
  return value;
}

constexpr void EraseCommaAndQuotation(std::string& s) {
  for (auto it = s.begin(); it != s.end(); ++it) {
    if (*it == ',' || *it == '\"') {
      s.erase(it);
    }
  }
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

}  // namespace utils

namespace graduate {

class Info {
 public:
  static constexpr std::string_view kDelimiter = "\t";
  enum StrSplitTable : size_t {
    kSchoolId = 0,
    kSchoolName,
    kDepartmentId,
    kDepartmentName,
    kDayOrNightType,
    kLevel,
    kStudentAmount,
    kTeacherAmount,
    kGraduateAmount,
    kCityName,
    kSchoolType
  };

  Info(std::string&& line, int serial_number) noexcept
      : data_{std::move(line)} {
    school_name_ = {utils::StrSplitAt(data_, kDelimiter, kSchoolName)};
    department_name_ = {utils::StrSplitAt(data_, kDelimiter, kDepartmentName)};
    day_or_night_type_ = {
        utils::StrSplitAt(data_, kDelimiter, kDayOrNightType)};
    level_ = {utils::StrSplitAt(data_, kDelimiter, kLevel)};
    serial_number_ = {serial_number};
    student_amount_ = {
        utils::StrToInt(utils::StrSplitAt(data_, kDelimiter, kStudentAmount))};
    graduate_amount_ = {
        utils::StrToInt(utils::StrSplitAt(data_, kDelimiter, kGraduateAmount))};
  }

  Info(std::string_view line, int serial_number) : data_{line} {
    school_name_ = {utils::StrSplitAt(data_, kDelimiter, kSchoolName)};
    department_name_ = {utils::StrSplitAt(data_, kDelimiter, kDepartmentName)};
    day_or_night_type_ = {
        utils::StrSplitAt(data_, kDelimiter, kDayOrNightType)};
    level_ = {utils::StrSplitAt(data_, kDelimiter, kLevel)};
    serial_number_ = {serial_number};
    student_amount_ = {
        utils::StrToInt(utils::StrSplitAt(data_, kDelimiter, kStudentAmount))};
    graduate_amount_ = {
        utils::StrToInt(utils::StrSplitAt(data_, kDelimiter, kGraduateAmount))};
  }

  void Println() const {
    std::println("[{}] {}, {}, {}, {}, {}, {}", serial_number_, school_name_,
                 department_name_, day_or_night_type_, level_, student_amount_,
                 graduate_amount_);
  }

  std::string_view school_name() const { return school_name_; }
  std::string_view department_name() const { return department_name_; }
  std::string_view day_or_night_type() const { return day_or_night_type_; }
  std::string_view level() const { return level_; }

 private:
  friend std::formatter<graduate::Info>;

  std::string data_;
  std::string_view school_name_;
  std::string_view department_name_;
  std::string_view day_or_night_type_;
  std::string_view level_;
  int serial_number_ = 0;
  int student_amount_ = 0;
  int graduate_amount_ = 0;
};

std::expected<std::vector<Info>, StatusCode> MakeList(
    const std::string& file_name) {
  std::ifstream file{file_name};

  if (!file.is_open()) {
    return std::unexpected{StatusCode::kNotFound};
  }

  auto skip_first_x_lines = [](std::istream& in, const int x) {
    assert(x > 0);
    for (int i = 0; i < x; i++) {
      in.ignore(10000, '\n');
    }
  };

  skip_first_x_lines(file, 3);

  std::string line;

  std::vector<Info> data;
  while (std::getline(file, line)) {
    data.emplace_back(std::move(line), data.size() + 1);
  }

  return data;
}

}  // namespace graduate

template <>
struct std::formatter<graduate::Info> : std::formatter<std::string> {
  auto format(const graduate::Info& val, std::format_context& context) const {
    return std::format_to(
        context.out(), "[{}] {}, {}, {}, {}, {}, {}", val.serial_number_,
        val.school_name_, val.department_name_, val.day_or_night_type_,
        val.level_, val.student_amount_, val.graduate_amount_);
  }
};

int main() {
  if (auto file_number = utils::Scan<std::string>("Enter file number: ");
      file_number.has_value()) [[likely]] {
    if (file_number.value() == "0") {
      return 0;
    }
    std::string file_name = std::format(
        "{}{}{}", kInputPrefix, std::move(file_number.value()), kInputSuffix);
    auto file_content = graduate::MakeList(file_name);
    if (file_content) {
      for (const auto& i : file_content.value()) {
        std::println("{}", i);
      }
    }
  }
}