// by 11227205 資訊二乙 劉至嘉 & 11027214 楊碕萍.
#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <expected>
#include <format>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#define NDEBUG
#define ENABLE_PRIME_NUMBER_LOOKUP_TABLE

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

constexpr std::string_view kInputPrefix = "input";
constexpr std::string_view kTextSuffix = ".txt";
constexpr std::string_view kBinarySuffix = ".bin";
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
  T value{};
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

struct Student {
  Student() = default;
  explicit Student(std::string_view line)
      : scores{StrTo<uint8_t>(StrSplitAt(line, "\t", 2)),
               StrTo<uint8_t>(StrSplitAt(line, "\t", 3)),
               StrTo<uint8_t>(StrSplitAt(line, "\t", 4)),
               StrTo<uint8_t>(StrSplitAt(line, "\t", 5)),
               StrTo<uint8_t>(StrSplitAt(line, "\t", 6)),
               StrTo<uint8_t>(StrSplitAt(line, "\t", 7))},
        score_average{StrTo<float>(StrSplitAt(line, "\t", 8))} {
    auto name = (StrSplitAt(line, "\t", 1));
    for (size_t i = 0; i < name.size(); ++i) {
      this->name[i] = name[i];
    }

    auto id = StrSplitAt(line, "\t", 0);
    for (size_t i = 0; i < id.size(); ++i) {
      this->id[i] = id[i];
    }
  }

  char id[10]{};
  char name[10]{};
  std::array<uint8_t, 6> scores{};
  float score_average{};
};

std::optional<std::vector<Student>> ReadFile(const std::string& file_name) {
  std::ifstream file{file_name};

  if (!file.is_open()) {
    return {};
  }

  std::vector<Student> infos;
  std::string line;
  while (std::getline(file, line)) {
    infos.emplace_back(line);
  }

  return infos;
}

void ReadFile(std::ifstream& in_file, std::vector<Student>& vec) {
  std::string line;
  while (std::getline(in_file, line)) {
    vec.emplace_back(line);
  }
}

void ReadFileBinary(std::ifstream& in_file, std::vector<Student>& vec) {
  Student s;
  while (in_file.read(reinterpret_cast<char*>(&s), sizeof(s))) {
    vec.emplace_back(std::move(s));
  }
}

std::optional<std::vector<Student>> ReadFileBinary(
    const std::string& file_name) {
  std::ifstream file{file_name, std::ios::binary};

  if (!file.is_open()) {
    return {};
  }

  std::vector<Student> infos;
  Student temp;

  while (file.read(reinterpret_cast<char*>(&temp), sizeof(temp))) {
    infos.emplace_back(std::move(temp));
  }

  return infos;
}

void WriteFileBinary(const std::string& file_name, std::span<Student> infos) {
  std::ofstream file{file_name, std::ios_base::binary};
  std::ranges::for_each(infos, [&file](const Student& i) {
    file.write(reinterpret_cast<const char*>(&i), sizeof(i));
  });
}

// 檢查質數
constexpr bool IsPrime(int num) {
  int divisor = 0;
  for (divisor = 2; divisor < num && num % divisor != 0; ++divisor) {
  }
  return divisor == num;
}  // IsPrime()

#ifdef ENABLE_PRIME_NUMBER_LOOKUP_TABLE
constexpr std::array<bool, 300> GeneratePrimeTable() {
  std::array<bool, 300> table;
  for (size_t i = 0; i < 300; ++i) {
    table[i] = IsPrime(i);
  }
  return table;
}

constexpr auto kPrimeTable = GeneratePrimeTable();
#endif

// 返回大於n的下一個質數
constexpr int NextPrime(int n) {
#ifdef ENABLE_PRIME_NUMBER_LOOKUP_TABLE
  if (n < 300) {
    for (; !kPrimeTable[n] && n < 300; ++n) {
    }
    return n;
  }
#endif
  for (; !IsPrime(n); ++n) {
  }
  return n;
}

class OpenAddressingHashMap {
 public:
  struct Bucket {
    size_t key{};
    Student data;
  };

  struct ProbeResult {
    size_t index{};
    int probe_amount{};
  };

  OpenAddressingHashMap() = default;
  explicit OpenAddressingHashMap(size_t size) : data_(size) {}

  int InsertRange(std::span<Student> range) {
    amount_of_students_ = range.size();
    size_t key;
    int probe_amount = 0;
    for (auto i : range) {
      key = Hash(i.id);
      const auto [index, probes] = Probe(key);
      probe_amount += probes;
      data_[index].emplace(key % data_.size(), std::move(i));
    }
    return probe_amount;
  }

  virtual ProbeResult Search(std::string_view id) const = 0;
  void Resize(size_t size) { data_.resize(size); }
  void Clear() { data_.clear(); }
  size_t size() const { return data_.size(); }

  const std::optional<Bucket>& operator[](size_t index) const {
    return data_[index];
  }

  size_t UnsuccessfulProbes() const {
    size_t total_unsuccess_prob = 0;
    for (size_t i = 0; i < data_.size(); ++i) {
      size_t index = i;
      // 嘗試搜尋不存在值會發生的 probing 次數.
      while (data_[index].has_value()) {
        ++total_unsuccess_prob;
        ++index;

        // 確保 index 在範圍內.
        if (index >= data_.size()) {
          index -= data_.size();
        }
      }
    }
    return total_unsuccess_prob;
  }

 protected:
  static size_t Hash(std::string_view student_id) noexcept {
    size_t key = 1;
    for (auto ch : student_id) {
      key *= ch;
    }
    return key;
  }

  virtual ProbeResult Probe(size_t key) const = 0;

  size_t amount_of_students_{};
  std::vector<std::optional<Bucket>> data_;
};

class LinearProbingHashMap : public OpenAddressingHashMap {
 public:
  ProbeResult Search(std::string_view id) const override {
    auto index = Hash(id) % data_.size();
    int probes = 1;
    while (data_[index].has_value()) {
      if (std::string_view{data_[index]->data.id} == id) {
        break;
      }
      ++index;
      ++probes;
      if (index >= data_.size()) {
        index -= data_.size();
      }
    }
    return {.index = index, .probe_amount = probes};
  }

 private:
  ProbeResult Probe(size_t key) const override {
    size_t index = key % data_.size();
    int successful_probes = 1;
    while (data_[index].has_value()) {
      ++index;
      ++successful_probes;
      if (index >= data_.size()) {
        index -= data_.size();
      }
    }
    return {.index = index, .probe_amount = successful_probes};
  }
};

class DoubleHashingHashMap : public OpenAddressingHashMap {
 public:
  ProbeResult Search(std::string_view id) const override {
    const auto key = Hash(id);
    auto index = key % data_.size();

    int highest_step = NextPrime(ceil(amount_of_students_ / 5.0));
    if (ceil(amount_of_students_ / 5.0) == amount_of_students_ / 5.0) {
      highest_step = NextPrime(ceil(amount_of_students_ / 5.0) + 1);
    }

    int step = highest_step - (key % highest_step);
    if (step == 0) {
      step = 1;
    }

    int total_success_prob = 1;
    while (data_[index].has_value()) {
      if (std::string_view{data_[index]->data.id} == id) {
        break;
      }
      ++total_success_prob;
      index += step;
      while (index >= data_.size()) {
        index -= data_.size();
      }
    }

    return {.index = index, .probe_amount = total_success_prob};
  }

 private:
  ProbeResult Probe(size_t key) const override {
    size_t index = key % data_.size();

    int highest_step = NextPrime(ceil(amount_of_students_ / 5.0));
    if (ceil(amount_of_students_ / 5.0) == amount_of_students_ / 5.0) {
      highest_step = NextPrime(ceil(amount_of_students_ / 5.0) + 1);
    }

    int step = highest_step - (key % highest_step);
    if (step == 0) {
      step = 1;
    }

    int total_success_prob = 1;
    while (data_[index].has_value()) {
      ++total_success_prob;
      index += step;
      while (index >= data_.size()) {
        index -= data_.size();
      }
    }
    return {.index = index, .probe_amount = total_success_prob};
  }
};

class HashTableSystem {
 public:
  static constexpr std::string_view kPrompt =
      "******* Hash Table *****\n"
      "* 0. QUIT              *\n"
      "* 1. Linear probing    *\n"
      "* 2. Double hashing    *\n"
      "************************\n"
      "Input a choice(0, 1, 2): ";

  StatusCode ParseCommand(std::string_view input) {
    if (!IsValidCommand(input)) {
      return StatusCode::kInvalidArgument;
    }
    const int command = StrTo<int>(input);
    switch (command) {
      case 0:
        return StatusCode::kCancelled;
      case 1: {
        return MakeHashSetWithLinearProbing();
      }
      case 2:
        return MakeHashSetWithDoubleHashing();
      default:
        return StatusCode::kInvalidArgument;
    }
    return StatusCode::kOk;
  }  // ParseInput()

 private:
  static bool IsValidCommand(std::string_view input) {
    return std::ranges::all_of(input, isdigit);
  }

  static void PrintStudent(
      std::ofstream& out_file,
      const std::optional<OpenAddressingHashMap::Bucket>& val) {
    if (val.has_value()) {
      const auto& v = val.value();
      std::print(out_file, "{:>10},{:>11},", v.key, v.data.id);
      // for some reason std::print can't reproduce the alignment of setw
      out_file << std::setw(11) << std::right << v.data.name;
      std::print(out_file, ",{:>11}", v.data.score_average);
    }
    out_file << '\n';
  }

  static void PrintSearchResult(
      const std::optional<OpenAddressingHashMap::Bucket>& val, int probes) {
    std::println("\n{{ {}, {}, {} }} is found after {} probes.\n", val->data.id,
                 val->data.name, val->data.score_average, probes);
  }

  StatusCode MakeHashSetWithLinearProbing() {
    students_.clear();
    linear_map_.Clear();

    // Step 1. 請使用者輸入檔案編號
    auto file_number = Scan<std::string>("\nInput a file number ([0] Quit): ");
    if (!file_number.has_value()) {
      return StatusCode::kInvalidArgument;
    }  // if()

    if (file_number.value() == "0") {
      return StatusCode::kOk;  // 直接離開
    }  // if()

    std::string bin_file_name = std::format("input{}.bin", file_number.value());
    std::string txt_file_name = std::format("input{}.txt", file_number.value());

    // Step 2. 讀取 bin 檔案

    std::ifstream bin_file{bin_file_name, std::ios_base::binary};
    if (bin_file.is_open()) {
      ReadFileBinary(bin_file, students_);
    } else {
      // bin 檔不存在
      std::println("\n### {} does not exist! ###", bin_file_name);

      // 試著打開 txt 檔
      std::ifstream txt_file{txt_file_name};
      if (txt_file.is_open()) {
        ReadFile(txt_file, students_);
      } else {
        // txt 也不存在
        std::println("\n### {} does not exist! ###\n\n", txt_file_name);
        return StatusCode::kNotFound;
      }

      // 轉存成 bin
      WriteFileBinary(bin_file_name, students_);
      std::println("\n---{} has been created ---", bin_file_name);
    }

    // Step 3. 取得學生資料
    linear_map_.Resize(NextPrime(ceil(students_.size() * 1.1)));

    // Step 4. 建立 hash table
    const int successful_probes = linear_map_.InsertRange(students_);

    // Step 6. 寫檔
    std::ofstream out(std::format("linear{}.txt", file_number.value()));

    std::println(out, "--- Hash table created by Linear probing    ---");

    for (size_t i = 0; i < linear_map_.size(); ++i) {
      std::print(out, "[{:3}] ", i);
      PrintStudent(out, linear_map_[i]);
    }

    std::println(out,
                 " ----------------------------------------------------- ");

    // Step 8. 輸出搜尋現存值(除以現存資料筆數)的平均比較次數
    std::println(
        "\nHash table has been successfully created by Linear probing   ");

    std::println("unsuccessful search: {:.4f} comparisons on average",
                 linear_map_.UnsuccessfulProbes() /
                     static_cast<float>(linear_map_.size()));
    std::println("successful search: {:.4f} comparisons on average",
                 successful_probes / static_cast<float>(students_.size()));

    file_name_ = file_number.value();

    auto search_id =
        Scan<std::string>("Input a student ID to search ([0] Quit): ");
    while (search_id && *search_id != "0") {
      const auto [index, probes] = linear_map_.Search(*search_id);
      if (linear_map_[index].has_value()) {
        PrintSearchResult(linear_map_[index], probes);
      } else {
        std::println("\n{} is not found after {} probes.\n", *search_id,
                     probes);
      }
      search_id =
          Scan<std::string>("Input a student ID to search ([0] Quit): ");
    }

      std::println("\n");
    

    return StatusCode::kOk;
  }  // MakeHashSetWithLinearProbing()

  StatusCode MakeHashSetWithDoubleHashing() {
    double_map_.Clear();
    if (students_.empty()) {
      std::println("### Command 1 first. ###\n\n");
      return StatusCode::kFailedPrecondition;
    }  // if()

    // Step1. 建立 hash table
    double_map_.Resize(NextPrime(ceil(students_.size() * 1.1)));

    // Step2. 把資料丟進 hash table
    const int total_success_prob = double_map_.InsertRange(students_);

    // Step3. 寫檔
    std::ofstream out(std::format("double{}.txt", file_name_));

    std::println(out, " --- Hash table created by Double hashing    ---");
    for (size_t i = 0; i < double_map_.size(); ++i) {
      std::print(out, "[{:3}] ", i);
      PrintStudent(out, double_map_[i]);
    }
    std::println(out,
                 " ----------------------------------------------------- ");

    // Step4. 輸出搜尋現存值(除以現存資料筆數)的平均比較次數
    std::println(
        "\nHash table has been successfully created by Double hashing   ");
    std::println("successful search: {:.4f} comparisons on average",
                 total_success_prob / static_cast<float>(students_.size()));

    auto search_id =
        Scan<std::string>("Input a student ID to search ([0] Quit): ");
    while (search_id && *search_id != "0") {
      const auto [index, probes] = double_map_.Search(*search_id);
      if (double_map_[index].has_value()) {
        PrintSearchResult(double_map_[index], probes);
      } else {
        std::println("{} is not found after {} probes.\n", *search_id,
                     probes);
      }
      search_id =
          Scan<std::string>("Input a student ID to search ([0] Quit): ");
    }
    std::println("\n");
    return StatusCode::kOk;
  }  // MakeHashSetWithDoubleHashing()

  std::vector<Student> students_;
  std::string file_name_;

  LinearProbingHashMap linear_map_;
  DoubleHashingHashMap double_map_;
};

}  // namespace

int main() {
  HashTableSystem system;

  for (auto status = StatusCode::kUnknown; status != StatusCode::kCancelled;) {
    auto user_input = Scan<std::string>(HashTableSystem::kPrompt);
    if (user_input.has_value()) {
      status = system.ParseCommand(user_input.value());
    }

    // Error handling
    if (status == StatusCode::kInvalidArgument) {
      std::println("\nCommand does not exist!\n\n");
    }
  }
}
