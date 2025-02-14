#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>


std::unique_ptr<void, void (*)(void*)> malloc_unique(size_t size);

bool starts_with(const std::string& s, const std::string& start);
bool ends_with(const std::string& s, const std::string& end);

std::string escape_quotes(const std::string& s);

std::string string_printf(const char* fmt, ...);
std::wstring wstring_printf(const wchar_t* fmt, ...);
std::string string_vprintf(const char* fmt, va_list va);
std::wstring wstring_vprintf(const wchar_t* fmt, va_list va);

uint8_t value_for_hex_char(char x);

#define DEBUG 0
#define INFO 1
#define WARNING 2
#ifndef WINDOWS
// wingdi.h defined ERROR as 0, lolz
#define ERROR 3
#endif

int log_level();
void set_log_level(int new_level);
void log(int level, const char* fmt, ...);

std::vector<std::string> split(const std::string& s, char delim);
std::vector<std::wstring> split(const std::wstring& s, wchar_t delim);
std::vector<std::string> split_context(const std::string& s, char delim);
std::string join(const std::vector<std::string>& items,
    const std::string& delim);

size_t skip_whitespace(const std::string& s, size_t offset);
size_t skip_whitespace(const char* s, size_t offset);
size_t skip_non_whitespace(const std::string& s, size_t offset);
size_t skip_non_whitespace(const char* s, size_t offset);
size_t skip_word(const std::string& s, size_t offset);
size_t skip_word(const char* s, size_t offset);

std::string string_for_error(int error);

enum class TerminalFormat {
  END        = -1,
  NORMAL     = 0,
  BOLD       = 1,
  UNDERLINE  = 4,
  BLINK      = 5,
  INVERSE    = 7,
  FG_BLACK   = 30,
  FG_RED     = 31,
  FG_GREEN   = 32,
  FG_YELLOW  = 33,
  FG_BLUE    = 34,
  FG_MAGENTA = 35,
  FG_CYAN    = 36,
  FG_GRAY    = 37,
  FG_WHITE   = 38,
  BG_BLACK   = 40,
  BG_RED     = 41,
  BG_GREEN   = 42,
  BG_YELLOW  = 43,
  BG_BLUE    = 44,
  BG_MAGENTA = 45,
  BG_CYAN    = 46,
  BG_GRAY    = 47,
  BG_WHITE   = 48,
};

std::string vformat_color_escape(TerminalFormat color, va_list va);
std::string format_color_escape(TerminalFormat color, ...);
void print_color_escape(FILE* stream, TerminalFormat color, ...);

void print_indent(FILE* stream, int indent_level);

enum PrintDataFlags {
  UseColor = 1, // use terminal escape codes to show differences
  PrintAscii = 2, // print ascii view on the right
  PrintFloat = 4, // print float view on the right
  PrintDouble = 8, // print double view on the right
  ReverseEndian = 16, // floats/doubles should be byteswapped
  CollapseZeroLines = 32, // skips lines of all zeroes
};

void print_data(FILE* stream, const void* _data, uint64_t size,
    uint64_t address = 0, const void* _prev = NULL,
    uint64_t flags = PrintDataFlags::PrintAscii);
void print_data(FILE* stream, const std::string& data, uint64_t address = 0,
    const void* prev = NULL, uint64_t flags = PrintDataFlags::PrintAscii);

std::string parse_data_string(const std::string& s, std::string* mask = NULL);
std::string format_data_string(const std::string& data, const std::string* mask = NULL);
std::string format_data_string(const void* data, size_t size, const void* mask = NULL);

std::string format_time(struct timeval* tv = NULL);
std::string format_size(size_t size, bool include_bytes = false);
size_t parse_size(const char* str);

class StringReader {
public:
  explicit StringReader(std::shared_ptr<std::string> data, size_t offset = 0);
  StringReader(const void* data, size_t size, size_t offset = 0);
  virtual ~StringReader() = default;

  size_t where() const;
  size_t size() const;
  void go(size_t offset);
  bool eof() const;
  std::string all() const;

  std::string read(size_t size, bool advance = true);
  std::string readx(size_t size, bool advance = true);
  size_t read_into(void* data, size_t size, bool advance = true);
  void readx_into(void* data, size_t size, bool advance = true);
  std::string pread(size_t offset, size_t size) const;
  std::string preadx(size_t offset, size_t size) const;
  size_t pread_into(size_t offset, void* data, size_t size) const;
  void preadx_into(size_t offset, void* data, size_t size) const;

  template <typename T> const T& get(bool advance = true) {
    if (this->offset > this->length - sizeof(T)) {
      throw std::out_of_range("end of string");
    }
    const T& ret = *reinterpret_cast<const T*>(this->data + this->offset);
    if (advance) {
      this->offset += sizeof(T);
    }
    return ret;
  }

  template <typename T> const T& pget(size_t offset) {
    if (offset > this->length - sizeof(T)) {
      throw std::out_of_range("end of string");
    }
    return *reinterpret_cast<const T*>(this->data + offset);
  }

  uint8_t get_u8(bool advance = true);
  int8_t get_s8(bool advance = true);
  uint16_t get_u16(bool advance = true);
  int16_t get_s16(bool advance = true);
  uint32_t get_u24(bool advance = true);
  int32_t get_s24(bool advance = true);
  uint32_t get_u32(bool advance = true);
  int32_t get_s32(bool advance = true);
  uint64_t get_u48(bool advance = true);
  int64_t get_s48(bool advance = true);
  uint64_t get_u64(bool advance = true);
  int64_t get_s64(bool advance = true);

  uint16_t get_u16r(bool advance = true);
  int16_t get_s16r(bool advance = true);
  uint32_t get_u24r(bool advance = true);
  int32_t get_s24r(bool advance = true);
  uint32_t get_u32r(bool advance = true);
  int32_t get_s32r(bool advance = true);
  uint64_t get_u48r(bool advance = true);
  int64_t get_s48r(bool advance = true);
  uint64_t get_u64r(bool advance = true);
  int64_t get_s64r(bool advance = true);

private:
  std::shared_ptr<std::string> owned_data;
  const uint8_t* data;
  size_t length;
  size_t offset;
};

class StringWriter {
public:
  StringWriter() = default;
  ~StringWriter() = default;

  size_t size() const;

  void write(const std::string& data);

  template <typename T> void put(T v) {
    this->data.append(reinterpret_cast<const char*>(&v), sizeof(v));
  }

  void put_u8(uint8_t v);
  void put_s8(int8_t v);
  void put_u16(uint16_t v);
  void put_s16(int16_t v);
  void put_u24(uint32_t v);
  void put_s24(int32_t v);
  void put_u32(uint32_t v);
  void put_s32(int32_t v);
  void put_u48(uint64_t v);
  void put_s48(int64_t v);
  void put_u64(uint64_t v);
  void put_s64(int64_t v);

  void put_u16r(uint16_t v);
  void put_s16r(int16_t v);
  void put_u24r(uint32_t v);
  void put_s24r(int32_t v);
  void put_u32r(uint32_t v);
  void put_s32r(int32_t v);
  void put_u48r(uint64_t v);
  void put_s48r(int64_t v);
  void put_u64r(uint64_t v);
  void put_s64r(int64_t v);

  inline std::string& str() {
    return data;
  }

private:
  std::string data;
};

template <typename T>
class StringBuffer : std::string {
public:
  StringBuffer(size_t size = sizeof(T)) : std::string(size, '\0') { }
  virtual ~StringBuffer() = default;

  T* buffer() {
    return reinterpret_cast<T*>(const_cast<char*>(this->data()));
  }
};

template <typename T>
T* data_at(std::string& s, size_t offset = 0) {
  return reinterpret_cast<T*>(const_cast<char*>(s.data() + offset));
}
