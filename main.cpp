#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

class Date
{
public:
    Date(int year_, int month_, int day) : year_(year_), month_(month_), day_(day) {}

    /// <summary>
    /// Calculate the number of days between two dates.
    /// </summary>
    /// <param name="date">another date</param>
    /// <returns>number of days</returns>
    auto operator-(const Date& date) const -> unsigned
    {
        // 0001/01/01 is day 1
        auto days_before_year = [](int year_) {
            assert(year_ >= 1);
            unsigned y = year_ - 1;
            return y * 365 + y / 4 - y / 100 + y / 400;
        };

        auto days_before_month = [](int year_, int month_) {
            assert(month_ >= 1 && month_ <= 12);
            unsigned days = days_before_months[month_];
            if (month_ >= 2 && IsLeapYear(year_)) {
                days++;
            }
            return days;
        };

        int days1 = days_before_year(year_) + days_before_month(year_, month_) + day_;
        int days2 = days_before_year(date.year_) + days_before_month(date.year_, date.month_) + date.day_;
        return abs(days1 - days2);
    }

    /// <summary>
    /// Determine if a year is a leap year.
    /// </summary>
    /// <param name="year">year</param>
    /// <returns>true if year is leap year</returns>
    static auto IsLeapYear(unsigned year_) -> bool { return year_ % 4 == 0 && (year_ % 100 != 0 || year_ % 400 == 0); }

    static constexpr unsigned MONTHS_OF_YEAR = 12;
    static constexpr unsigned DAYS_OF_WEEK = 7;

private:
    const int year_;
    const int month_;
    const int day_;

    static std::array<int, MONTHS_OF_YEAR + 1> days_before_months;
};

std::array<int, Date::MONTHS_OF_YEAR + 1> Date::days_before_months{0,   0,   31,  59,  90,  120, 151,
                                                                   181, 212, 243, 273, 304, 334};

class MonthPrinter
{
public:
    MonthPrinter(unsigned year_, unsigned month_) : year_(year_), month_(month_)
    {
        output_start_day_ = MONTH_START_DAY;
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
        case 2: output_end_day_ = static_cast<unsigned>(Date::IsLeapYear(year_)) + 28U; break;
        default: break;
        }
    }

    /// <summary>
    /// Centrally display the month name.
    /// </summary>
    void PrintMonthName()
    {
        const std::string& month_name = months_display_name[month_];
        auto size = static_cast<unsigned>(month_name.length());

        unsigned prefix_spaces_length;
        unsigned suffix_spaces_length;
        if ((LINE_MAX_LENGTH - size) % 2 == 0) {
            prefix_spaces_length = suffix_spaces_length = (LINE_MAX_LENGTH - size) >> 1;
        } else {
            suffix_spaces_length = (LINE_MAX_LENGTH - size) >> 1;
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
    void PrintWeekName() { std::cout << " Su Mo Tu We Th Fr Sa "; }

    /// <summary>
    /// Print date numbers line by line when call this function.
    /// </summary>
    /// <returns>true if any number was printed.</returns>
    auto PrintDates() -> bool
    {
        if (output_start_day_ > output_end_day_) {
            for (unsigned i = 0; i < LINE_MAX_LENGTH; i++) {
                std::cout << ' ';
            }
            return false;
        }

        const std::string spaces_literal("   ");
        const Date standard_date(1970, 2, 1);

        if (output_start_day_ == MONTH_START_DAY) {
            // The first line
            Date current_date(year_, month_, 1);
            unsigned spaces = (current_date - standard_date) % 7;

            for (unsigned i = 0; i < spaces; i++) {
                std::cout << spaces_literal;
            }

            for (unsigned i = 0; i < LINE_MAX_COUNT - spaces; i++) {
                printf("%3d", output_start_day_++);
            }
        } else if (output_start_day_ + 6 <= output_end_day_) {
            for (unsigned i = 0; i < LINE_MAX_COUNT; i++) {
                printf("%3d", output_start_day_++);
            }
        } else {
            // The last line
            unsigned spaces = LINE_MAX_COUNT - 1 - (output_end_day_ - output_start_day_);

            for (unsigned i = 0; i < LINE_MAX_COUNT - spaces; i++) {
                printf("%3d", output_start_day_++);
            }

            for (unsigned i = 0; i < spaces; i++) {
                std::cout << spaces_literal;
            }
        }
        std::cout << ' ';

        return true;
    }

private:
    static std::array<const std::string, Date::MONTHS_OF_YEAR + 1> months_display_name;

    static constexpr unsigned LINE_MAX_LENGTH = 22;
    static constexpr unsigned LINE_MAX_COUNT = 7;
    static constexpr unsigned MONTH_START_DAY = 1;

    uint16_t output_start_day_;
    uint16_t output_end_day_;
    unsigned year_;
    unsigned month_;
};

std::array<const std::string, Date::MONTHS_OF_YEAR + 1> MonthPrinter::months_display_name{
    "", // 1-based index
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"};

class PrinterProxy
{
public:
    static void PrintMonth(unsigned year_, unsigned month_) { PrintMonthsVec(year_, {month_}); }

    static void PrintMonthsVec(unsigned year_, const std::vector<unsigned>& months)
    {
        // Construct month printers
        std::vector<MonthPrinter> printers;
        auto size = static_cast<unsigned>(months.size());
        for (unsigned i = 0; i < size; i++) {
            printers.emplace_back(year_, months[i]);
        }

        // Start printing
        for (unsigned i = 0; i < size; i++) {
            printers[i].PrintMonthName();
        }
        std::cout << std::endl;

        for (unsigned i = 0; i < size; i++) {
            printers[i].PrintWeekName();
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

auto main(int argc, char* argv[]) -> int
{
    if (argc < 2) {
        std::cerr << "Invalid args." << std::endl;
        exit(EXIT_FAILURE);
    }

    uint32_t year = std::stoul(argv[1]);

    // Print 3 month in a row by default
    for (unsigned i = 0; i < 4; i++) {
        unsigned start_month = i * 3 + 1;
        PrinterProxy::PrintMonthsVec(year, {start_month, start_month + 1, start_month + 2});
    }
}
