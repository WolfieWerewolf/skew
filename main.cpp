// node skewc.js src/*/*.sk --verbose --target=cpp
// clang++ -O3 main.cpp -std=c++11 -ferror-limit=0 -DNDEBUG

#include <string>
#include <vector>
#include <unordered_map>
#include <initializer_list>

using string = std::string;

template <typename T>
struct IComparison;

template <typename T>
struct ListCompare {
  ListCompare(IComparison<T> *comparison) : comparison(comparison) {}
  bool operator () (T left, T right) const { return comparison->compare(left, right) < 0; }
  IComparison<T> *comparison;
};

template <typename T>
struct List {
  List() {}
  List(std::initializer_list<T> list) : data(std::move(list)) {}
  int size() { return data.size(); }
  void push(T value) { data.push_back(value); }
  void unshift(T value) { data.insert(data.begin(), value); }
  List<T> *slice(int start, int end) { auto value = new List<T>(); if (start < end) value->data.insert(value->data.begin(), data.begin() + start, data.begin() + end); return value; }
  int indexOf(T value) { auto it = std::find(data.begin(), data.end(), value); return it != data.end() ? it - data.begin() : -1; }
  int lastIndexOf(T value) { auto it = std::find(data.rbegin(), data.rend(), value); return data.size() - 1 - (it - data.rbegin()); }
  T shift() { T value = data.front(); data.erase(data.begin()); return value; }
  T pop() { T value = data.back(); data.pop_back(); return value; }
  void reverse() { std::reverse(data.begin(), data.end()); }
  void sort(IComparison<T> *comparison) { std::sort(data.begin(), data.end(), ListCompare<T>(comparison)); }
  List<T> *clone() { return slice(0, size()); }
  T remove(int index) { T value = data[index]; data.erase(data.begin() + index); return value; }
  void insert(int index, T value) { data.insert(data.begin() + index, value); }
  T get(int index) { return data[index]; }
  void set(int index, T value) { data[index] = value; }
  void swap(int a, int b) { std::swap(data[a], data[b]); }

private:
  std::vector<T> data;
};

template <typename T>
struct StringMap {
  StringMap() {}
  T get(string key) { return data[key]; }
  T getOrDefault(string key, T defaultValue) { auto it = data.find(key); return it != data.end() ? it->second : defaultValue; }
  void set(string key, T value) { data[key] = value; }
  bool has(string key) { return data.count(key); }
  void remove(string key) { data.erase(key); }
  List<string> *keys() { auto keys = new List<string>(); for (auto &it : data) keys->push(it.first); return keys; }
  List<T> *values() { auto values = new List<T>(); for (auto &it : data) values->push(it.second); return values; }
  StringMap<T> *clone() { auto clone = new StringMap<T>(); clone->data = data; return clone; }

private:
  std::unordered_map<string, T> data;
};

template <typename T>
struct IntMap {
  IntMap() {}
  T get(int key) { return data[key]; }
  T getOrDefault(int key, T defaultValue) { auto it = data.find(key); return it != data.end() ? it->second : defaultValue; }
  void set(int key, T value) { data[key] = value; }
  bool has(int key) { return data.count(key); }
  void remove(int key) { data.erase(key); }
  List<int> *keys() { auto keys = new List<int>(); for (auto &it : data) keys->push(it.first); return keys; }
  List<T> *values() { auto values = new List<T>(); for (auto &it : data) values->push(it.second); return values; }
  IntMap<T> *clone() { auto clone = new IntMap<T>(); clone->data = data; return clone; }

private:
  std::unordered_map<int, T> data;
};

double now();
string encodeBase64(string text);
double parseDoubleLiteral(string text);
double parseIntLiteral(string text, int base);
string cpp_toString(int value);
string cpp_toString(bool value);
string cpp_toString(double value);
string cpp_fromCodeUnit(int value);
string cpp_toLowerCase(string value);
string cpp_toUpperCase(string value);
string cpp_slice(const string &value, int start, int end);

struct Source;

namespace in_io {
  enum class Color;
}

namespace io {
  extern int terminalWidth;
  void setColor(in_io::Color color);
  void print(string text);
  bool writeFile(string path, string contents);
  Source *readFile(string path);
}

#include "output.cpp"

#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/time.h>
#include <iomanip>

double now() {
  timeval data;
  gettimeofday(&data, NULL);
  return data.tv_sec * 1.0e3 + data.tv_usec / 1.0e3;
}

string encodeBase64(string text) {
  static const char *BASE64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  string result;
  int n = text.size();
  int i;
  for (i = 0; i + 2 < n; i = i + 3) {
    int c = text[i] << 16 | text[i + 1] << 8 | text[i + 2];
    result += BASE64[c >> 18];
    result += BASE64[c >> 12 & 0x3F];
    result += BASE64[c >> 6 & 0x3F];
    result += BASE64[c & 0x3F];
  }
  if (i < n) {
    int a = text[i];
    result += BASE64[a >> 2];
    if (i + 1 < n) {
      int b = text[i + 1];
      result += BASE64[(a << 4 & 0x30) | b >> 4];
      result += BASE64[b << 2 & 0x3C];
      result += "=";
    } else {
      result += BASE64[a << 4 & 0x30];
      result += "==";
    }
  }
  return result;
}

double parseDoubleLiteral(string text) {
  double value = 0;
  std::stringstream ss(text);
  ss >> value;
  return value;
}

double parseIntLiteral(string text, int base) {
  int value = 0;
  std::stringstream ss(base == 10 ? text : text.substr(2));
  ss >> std::setbase(base) >> value;
  return value;
}

string cpp_toString(int value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

string cpp_toString(bool value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

string cpp_toString(double value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

string cpp_fromCodeUnit(int value) {
  return string(1, value);
}

string cpp_toLowerCase(string value) {
  std::transform(value.begin(), value.end(), value.begin(), tolower);
  return value;
}

string cpp_toUpperCase(string value) {
  std::transform(value.begin(), value.end(), value.begin(), toupper);
  return value;
}

string cpp_slice(const string &value, int start, int end) {
  return value.substr(start, end - start);
}

bool isTTY = false;
int io::terminalWidth = 0;

void io::setColor(in_io::Color color) {
  if (isTTY) std::cout << "\e[" << (int)color << 'm';
}

void io::print(string text) {
  std::cout << text;
}

bool io::writeFile(string path, string contents) {
  std::ofstream file(path.c_str());
  if (!file) return false;
  file << contents;
  return true;
}

Source *io::readFile(string path) {
  std::ifstream file(path.c_str());
  if (!file) return nullptr;
  return new Source(path, std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
}

int main(int argc, char *argv[]) {
  auto args = new List<string>();
  for (int i = 1; i < argc; i++) {
    args->push(argv[i]);
  }
  winsize size;
  if (!ioctl(2, TIOCGWINSZ, &size)) {
    io::terminalWidth = size.ws_col;
  }
  isTTY = isatty(STDOUT_FILENO);
  return frontend::main(args);
}

#include <sys/mman.h>

// Replace the standard malloc() implementation with a bump allocator for
// speed. Never freeing anything is totally fine for a short-lived process.
// This gives a significant speedup.
extern "C" void *malloc(size_t size) {
  enum { CHUNK_SIZE = 1 << 20 };

  static void *next;
  static size_t available;

  if (available < size) {
    size_t chunk = (size + CHUNK_SIZE - 1) / CHUNK_SIZE * CHUNK_SIZE;
    assert(size <= chunk);
    next = mmap(NULL, chunk, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    assert(next != MAP_FAILED);
    available = chunk;
  }

  void *data = next;
  next = (uint8_t *)next + size;
  available -= size;
  return data;
}

extern "C" void free(void *data) {}
void *operator new (size_t size) { return malloc(size); }
void *operator new [] (size_t size) { return malloc(size); }
void operator delete (void *data) throw() {}
void operator delete [] (void *data) throw() {}