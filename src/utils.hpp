// Copyright (C) 2022-2024 Vladislav Nepogodin
//
// This file is part of CachyOS kernel manager.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#ifndef UTILS_HPP
#define UTILS_HPP

#include <span>         // for span
#include <string>       // for string
#include <string_view>  // for string_view
#include <vector>       // for vector

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wnull-dereference"
#pragma GCC diagnostic ignored "-Wsuggest-attribute=pure"
#endif

#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>

#include <QString>

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <alpm.h>

namespace utils {

[[nodiscard]] auto read_whole_file(const std::string_view& filepath) noexcept -> std::string;
bool write_to_file(const std::string_view& filepath, const std::string_view& data) noexcept;
std::string exec(const std::string_view& command) noexcept;
[[nodiscard]] std::string fix_path(std::string&& path) noexcept;

/// @brief Make a split view from a string into multiple lines based on a delimiter.
/// @param str The string to split.
/// @param delim The delimiter to split the string.
/// @return A range view representing the split lines.
constexpr auto make_split_view(std::string_view str, char delim) noexcept {
    constexpr auto functor = [](auto&& rng) {
        return std::string_view(&*rng.begin(), static_cast<size_t>(ranges::distance(rng)));
    };
    constexpr auto second = [](auto&& rng) { return rng != ""; };

    return str
        | ranges::views::split(delim)
        | ranges::views::transform(functor)
        | ranges::views::filter(second);
}

alpm_handle_t* parse_alpm(std::string_view root, std::string_view dbpath, alpm_errno_t* err) noexcept;
std::int32_t release_alpm(alpm_handle_t* handle, alpm_errno_t* err) noexcept;

// Runs a command in a terminal, escalates using pkexec if escalate is true
int runCmdTerminal(QString cmd, bool escalate) noexcept;

void prepare_build_environment() noexcept;
void restore_clean_environment(std::vector<std::string>& previously_set_options, std::string_view all_set_values) noexcept;

inline constexpr std::size_t replace_all(std::string& inout, std::string_view what, std::string_view with) noexcept {
    std::size_t count{};
    std::size_t pos{};
    while (std::string::npos != (pos = inout.find(what.data(), pos, what.length()))) {
        inout.replace(pos, what.length(), with.data(), with.length());
        pos += with.length(), ++count;
    }
    return count;
}

inline constexpr std::size_t remove_all(std::string& inout, std::string_view what) noexcept {
    return replace_all(inout, what, "");
}

[[nodiscard]] inline constexpr auto make_multiline(std::string_view str, char delim = '\n') noexcept -> std::vector<std::string> {
    return [&]() constexpr {
        constexpr auto functor = [](auto&& rng) {
            return std::string_view(&*rng.begin(), static_cast<size_t>(ranges::distance(rng)));
        };
        constexpr auto second = [](auto&& rng) { return rng != ""; };

        auto&& view_res = str
            | ranges::views::split(delim)
            | ranges::views::transform(functor);

        std::vector<std::string> lines{};
        ranges::for_each(view_res | ranges::views::filter(second), [&](auto&& rng) { lines.emplace_back(rng); });
        return lines;
    }();
}

[[nodiscard]] inline constexpr auto make_multiline_view(std::string_view str, char delim) noexcept -> std::vector<std::string_view> {
    return [&]() constexpr {
        constexpr auto functor = [](auto&& rng) {
            return std::string_view(&*rng.begin(), static_cast<size_t>(ranges::distance(rng)));
        };
        constexpr auto second = [](auto&& rng) { return rng != ""; };

        auto&& view_res = str
            | ranges::views::split(delim)
            | ranges::views::transform(functor);

        std::vector<std::string_view> lines{};
        ranges::for_each(view_res | ranges::views::filter(second), [&](auto&& rng) { lines.emplace_back(rng); });
        return lines;
    }();
}

[[nodiscard]] inline constexpr auto join_vec(std::span<std::string_view> lines, std::string_view delim) noexcept -> std::string {
    return [&] { return lines | ranges::views::join(delim) | ranges::to<std::string>(); }();
}

}  // namespace utils

#endif  // UTILS_HPP
