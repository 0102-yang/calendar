#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

class Date
{
public:
    Date(const int year, const int month, const int day) : year_(year), month_(month), day_(day) {}

    /// <summary>
    /// Calculate the number of days between two dates.
    /// </summary>
    /// <param name="date">another date</param>
    /// <returns>number of days</returns>
    auto operator-(const Date& date) const -> unsigned
    {
        // 0001/01/01 is day 1
        auto days_before_year = [](const int year) {
            assert(year >= 1);
            const int y = year - 1;
            return y * 365 + y / 4 - y / 100 + y / 400;
        };

        auto days_before_month = [](const int year, const int month) {
            assert(month >= 1 && month <= 12);
            auto days = days_before_months_[month];
            if (month >= 2 && IsLeapYear(year)) {
                days++;
            }
            return days;
        };

        const auto days1 = days_before_year(year_) + days_before_month(year_, month_) + day_;
        const auto days2 = days_before_year(date.year_) + days_before_month(date.year_, date.month_) + date.day_;
        return abs(days1 - days2);
    }

    /// <summary>
    /// Determine if a year is a leap year.
    /// </summary>
    /// <param name="year">year</param>
    /// <returns>true if year is leap year</returns>
    static auto IsLeapYear(const unsigned year) -> bool
    {
        return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    }

    static constexpr unsigned months_of_year = 12;
    static constexpr unsigned days_of_week = 7;

private:
    const int year_;
    const int month_;
    const int day_;

    static std::array<int, months_of_year + 1> days_before_months_;
};

std::array<int, Date::months_of_year + 1> Date::days_before_months_{0,   0,   31,  59,  90,  120, 151,
                                                                    181, 212, 243, 273, 304, 334};

class MonthPrinter
{
public:
    MonthPrinter(const int year, const int month) : year_(year), month_(month)
    {
        output_start_day_ = month_start_day;
        switch (month_) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12: output_end_day_ = 31; break;
        case 4:
        case 6:
        case 9:
        case 11: output_end_day_ = 30; break;
        case 2: output_end_day_ = static_cast<int16_t>(Date::IsLeapYear(year_) + 28); break;
        default: break;
        }
    }

    /// <summary>
    /// Centrally display the month name.
    /// </summary>
    void PrintMonthName() const
    {
        const std::string& month_name = months_display_name_[month_];
        const auto month_name_length = static_cast<unsigned>(month_name.length());
        const auto spaces_length = line_max_length - month_name_length;

        // Calculate prefix and suffix spaces length
        unsigned prefix_spaces_length;
        unsigned suffix_spaces_length;
        if (spaces_length % 2 == 0) {
            prefix_spaces_length = suffix_spaces_length = spaces_length >> 1;
        } else {
            suffix_spaces_length = spaces_length >> 1;
            prefix_spaces_length = suffix_spaces_length + 1;
        }

        for (unsigned i = 0; i < prefix_spaces_length; i++) {
            std::cout << ' ';
        }
        std::cout << month_name;
        for (unsigned i = 0; i < suffix_spaces_length; i++) {
            std::cout << ' ';
        }
    }

    /// <summary>
    /// Print week header.
    /// </summary>
    static void PrintWeekName() { std::cout << " Su Mo Tu We Th Fr Sa "; }

    /// <summary>
    /// Print date numbers line by line when call this function.
    /// </summary>
    /// <returns>true if any number was printed.</returns>
    auto PrintDates() -> bool
    {
        if (output_start_day_ > output_end_day_) {
            for (unsigned i = 0; i < line_max_length; i++) {
                std::cout << ' ';
            }
            return false;
        }

        const std::string spaces_literal("   ");
        const Date standard_date(1970, 2, 1);

        if (output_start_day_ == month_start_day) {
            // The first line
            const Date current_date(year_, month_, 1);
            const unsigned spaces = (current_date - standard_date) % 7;

            for (unsigned i = 0; i < spaces; i++) {
                std::cout << spaces_literal;
            }

            for (unsigned i = 0; i < line_max_count - spaces; i++) {
                printf("%3u", output_start_day_++);
            }
        } else if (output_start_day_ + 6 <= output_end_day_) {
            for (unsigned i = 0; i < line_max_count; i++) {
                printf("%3u", output_start_day_++);
            }
        } else {
            // The last line
            const unsigned spaces = line_max_count - 1 - (output_end_day_ - output_start_day_);

            for (unsigned i = 0; i < line_max_count - spaces; i++) {
                printf("%3u", output_start_day_++);
            }

            for (unsigned i = 0; i < spaces; i++) {
                std::cout << spaces_literal;
            }
        }
        std::cout << ' ';

        return true;
    }

private:
    static std::array<const std::string, Date::months_of_year + 1> months_display_name_;

    static constexpr int line_max_length = 22;
    static constexpr int line_max_count = 7;
    static constexpr int month_start_day = 1;

    int16_t output_start_day_;
    int16_t output_end_day_;
    int year_;
    int month_;
};

std::array<const std::string, Date::months_of_year + 1> MonthPrinter::months_display_name_{
    "", // 1-based index
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"};

class PrinterProxy
{
public:
    static void PrintMonth(const int year, const int month) { PrintMonthsVec(year, {month}); }

    static void PrintMonthsVec(const int year, const std::vector<int>& months)
    {
        // Construct month printers
        std::vector<MonthPrinter> printers;
        const auto size = static_cast<unsigned>(months.size());
        for (unsigned i = 0; i < size; i++) {
            printers.emplace_back(year, months[i]);
        }

        // Start printing
        for (unsigned i = 0; i < size; i++) {
            printers[i].PrintMonthName();
        }
        std::cout << std::endl;

        for (unsigned i = 0; i < size; i++) {
            MonthPrinter::PrintWeekName();
        }
        std::cout << std::endl;

        bool status = true;
        while (status) {
            for (unsigned i = 0; i < size; i++) {
                status &= printers[i].PrintDates();
            }
            std::cout << std::endl;

            if (!status) {
                break;
            }
        }
        std::cout << std::endl;
    }
};

auto main(const int argc, char* argv[]) -> int
{
    if (argc < 2) {
        std::cerr << "Invalid args." << std::endl;
        exit(EXIT_FAILURE);
    }

    const auto year = static_cast<int>(std::stol(argv[1]));

    // Print 3 month in a row by default
    for (unsigned i = 0; i < 4; i++) {
        auto start_month = static_cast<int>(i * 3 + 1);
        PrinterProxy::PrintMonthsVec(year, {start_month, start_month + 1, start_month + 2});
    }
}
