#pragma once

#include "momentary_switch_event/apple_vendor_keyboard_key_code.hpp"
#include "momentary_switch_event/apple_vendor_top_case_key_code.hpp"
#include "momentary_switch_event/consumer_key_code.hpp"
#include "momentary_switch_event/key_code.hpp"
#include "momentary_switch_event/pointing_button.hpp"
#include <nlohmann/json.hpp>
#include <pqrs/hash.hpp>
#include <variant>

namespace krbn {
/// Events from momentary switch hardwares such as key, consumer, pointing_button.
class momentary_switch_event final {
public:
  using value_t = std::variant<key_code::value_t,
                               consumer_key_code::value_t,
                               apple_vendor_keyboard_key_code::value_t,
                               apple_vendor_top_case_key_code::value_t,
                               pointing_button::value_t,
                               std::monostate>;

  momentary_switch_event(void) : value_(std::monostate()) {
  }

  template <typename T>
  explicit momentary_switch_event(const T& value) : value_(value) {
  }

  explicit momentary_switch_event(const pqrs::hid::usage_pair& usage_pair) : value_(std::monostate()) {
    if (auto key_code = make_key_code(usage_pair.get_usage_page(), usage_pair.get_usage())) {
      value_ = *key_code;
    } else if (auto consumer_key_code = make_consumer_key_code(usage_pair.get_usage_page(), usage_pair.get_usage())) {
      value_ = *consumer_key_code;
    } else if (auto apple_vendor_keyboard_key_code = make_apple_vendor_keyboard_key_code(usage_pair.get_usage_page(), usage_pair.get_usage())) {
      value_ = *apple_vendor_keyboard_key_code;
    } else if (auto apple_vendor_top_case_key_code = make_apple_vendor_top_case_key_code(usage_pair.get_usage_page(), usage_pair.get_usage())) {
      value_ = *apple_vendor_top_case_key_code;
    } else if (auto pointing_button = make_pointing_button(usage_pair.get_usage_page(), usage_pair.get_usage())) {
      value_ = *pointing_button;
    }
  }

  const value_t& get_value(void) const {
    return value_;
  }

  template <typename T>
  void set_value(T value) {
    value_ = value;
  }

  template <typename T>
  const T* find(void) const {
    return std::get_if<T>(&value_);
  }

  std::optional<pqrs::hid::usage_pair> make_usage_pair(void) const {
    std::optional<pqrs::hid::usage_page::value_t> usage_page;
    std::optional<pqrs::hid::usage::value_t> usage;

    if (auto value = find<key_code::value_t>()) {
      usage_page = make_hid_usage_page(*value);
      usage = make_hid_usage(*value);
    } else if (auto value = find<consumer_key_code::value_t>()) {
      usage_page = make_hid_usage_page(*value);
      usage = make_hid_usage(*value);
    } else if (auto value = find<apple_vendor_keyboard_key_code::value_t>()) {
      usage_page = make_hid_usage_page(*value);
      usage = make_hid_usage(*value);
    } else if (auto value = find<apple_vendor_top_case_key_code::value_t>()) {
      usage_page = make_hid_usage_page(*value);
      usage = make_hid_usage(*value);
    } else if (auto value = find<pointing_button::value_t>()) {
      usage_page = make_hid_usage_page(*value);
      usage = make_hid_usage(*value);
    }

    if (usage_page && usage) {
      return pqrs::hid::usage_pair(*usage_page, *usage);
    }

    return std::nullopt;
  }

  std::optional<krbn::modifier_flag> make_modifier_flag(void) const {
    if (auto usage_pair = make_usage_pair()) {
      auto usage_page = usage_pair->get_usage_page();
      auto usage = usage_pair->get_usage();

      if (usage_page == pqrs::hid::usage_page::keyboard_or_keypad) {
        if (usage == pqrs::hid::usage::keyboard_or_keypad::keyboard_left_control) {
          return krbn::modifier_flag::left_control;
        } else if (usage == pqrs::hid::usage::keyboard_or_keypad::keyboard_left_shift) {
          return krbn::modifier_flag::left_shift;
        } else if (usage == pqrs::hid::usage::keyboard_or_keypad::keyboard_left_alt) {
          return krbn::modifier_flag::left_option;
        } else if (usage == pqrs::hid::usage::keyboard_or_keypad::keyboard_left_gui) {
          return krbn::modifier_flag::left_command;
        } else if (usage == pqrs::hid::usage::keyboard_or_keypad::keyboard_right_control) {
          return krbn::modifier_flag::right_control;
        } else if (usage == pqrs::hid::usage::keyboard_or_keypad::keyboard_right_shift) {
          return krbn::modifier_flag::right_shift;
        } else if (usage == pqrs::hid::usage::keyboard_or_keypad::keyboard_right_alt) {
          return krbn::modifier_flag::right_option;
        } else if (usage == pqrs::hid::usage::keyboard_or_keypad::keyboard_right_gui) {
          return krbn::modifier_flag::right_command;
        }
      } else if (usage_page == pqrs::hid::usage_page::apple_vendor_keyboard) {
        if (usage == pqrs::hid::usage::apple_vendor_keyboard::function) {
          return krbn::modifier_flag::fn;
        }
      } else if (usage_page == pqrs::hid::usage_page::apple_vendor_top_case) {
        if (usage == pqrs::hid::usage::apple_vendor_top_case::keyboard_fn) {
          return krbn::modifier_flag::fn;
        }
      }
    }

    return std::nullopt;
  }

  bool modifier_flag(void) const {
    return make_modifier_flag() != std::nullopt;
  }

  bool pointing_button(void) const {
    return std::holds_alternative<pointing_button::value_t>(value_);
  }

  bool operator==(const momentary_switch_event& other) const {
    return value_ == other.value_;
  }

  bool operator<(const momentary_switch_event& other) const {
    return value_ < other.value_;
  }

private:
  value_t value_;
};

inline void to_json(nlohmann::json& json, const momentary_switch_event& value) {
  if (auto v = value.find<key_code::value_t>()) {
    json["key_code"] = make_key_code_name(*v);

  } else if (auto v = value.find<consumer_key_code::value_t>()) {
    json["consumer_key_code"] = make_consumer_key_code_name(*v);

  } else if (auto v = value.find<apple_vendor_keyboard_key_code::value_t>()) {
    json["apple_vendor_keyboard_key_code"] = make_apple_vendor_keyboard_key_code_name(*v);

  } else if (auto v = value.find<apple_vendor_top_case_key_code::value_t>()) {
    json["apple_vendor_top_case_key_code"] = make_apple_vendor_top_case_key_code_name(*v);

  } else if (auto v = value.find<pointing_button::value_t>()) {
    json["pointing_button"] = make_pointing_button_name(*v);
  }
}

inline void from_json(const nlohmann::json& json, momentary_switch_event& value) {
  pqrs::json::requires_object(json, "json");

  for (const auto& [k, v] : json.items()) {
    if (k == "key_code") {
      value.set_value(v.get<key_code::value_t>());

    } else if (k == "consumer_key_code") {
      value.set_value(v.get<consumer_key_code::value_t>());

    } else if (k == "apple_vendor_keyboard_key_code") {
      value.set_value(v.get<apple_vendor_keyboard_key_code::value_t>());

    } else if (k == "apple_vendor_top_case_key_code") {
      value.set_value(v.get<apple_vendor_top_case_key_code::value_t>());

    } else if (k == "pointing_button") {
      value.set_value(v.get<pointing_button::value_t>());

    } else {
      throw pqrs::json::unmarshal_error(fmt::format("unknown key: `{0}`", k));
    }
  }
}
} // namespace krbn

namespace std {
template <>
struct hash<krbn::momentary_switch_event> final {
  std::size_t operator()(const krbn::momentary_switch_event& value) const {
    std::size_t h = 0;

    pqrs::hash::combine(h, value.get_value());

    return h;
  }
};
} // namespace std
