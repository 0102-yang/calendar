#include <array>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Date
{
public:
    Date(unsigned year, unsigned month, unsigned day) : m_year(year), m_month(month), m_day(day) {}

    /// <summary>
    /// Calculate the number of days between two dates.
    /// </summary>
    /// <param name="date">another date</param>
    /// <returns>number of days</returns>
    unsigned operator-(const Date& date) const
    {
        unsigned smallerYear = min(m_year, date.m_year);

        auto offsetDaysBetweenYears = [](unsigned smallerYear, unsigned LargerYear) -> unsigned {
            if (smallerYear == LargerYear) return 0;

            unsigned days = 0;
            for (; smallerYear < LargerYear; smallerYear++) days += isLeapYear(smallerYear) ? 366 : 365;

            return days;
        };

        auto offsetDaysFromJanToMonth = [](unsigned year, unsigned month) {
            unsigned days = 0;
            for (unsigned i = 1; i < month; i++) {
                if (i == 2 && isLeapYear(year)) days += SPECIAL_DAY;

                days += sm_totalDaysOfNormalYearMonth[i - 1];
            }
            return days;
        };

        unsigned offsetDay1 = 0, offsetDay2 = 0;
        // Compute offsetDay1
        offsetDay1 += offsetDaysBetweenYears(smallerYear, m_year);
        offsetDay1 += offsetDaysFromJanToMonth(m_year, m_month);
        offsetDay1 += m_day - 1;
        // Compute offsetDay2
        offsetDay2 += offsetDaysBetweenYears(smallerYear, date.m_year);
        offsetDay2 += offsetDaysFromJanToMonth(date.m_year, date.m_month);
        offsetDay2 += date.m_day - 1;

        return max(offsetDay1, offsetDay2) - min(offsetDay1, offsetDay2);
    }

    /// <summary>
    /// Determine if a year is a leap year.
    /// </summary>
    /// <param name="year">year</param>
    /// <returns>true if year is leap year</returns>
    static bool isLeapYear(unsigned year) { return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0; }

    static constexpr unsigned MONTHS_OF_YEAR = 12;
    static constexpr unsigned DAYS_OF_WEEK = 7;

private:
    const unsigned m_year;
    const unsigned m_month;
    const unsigned m_day;

    static array<unsigned, MONTHS_OF_YEAR> sm_totalDaysOfNormalYearMonth;
    static constexpr unsigned SPECIAL_DAY = 29;
};

array<unsigned, Date::MONTHS_OF_YEAR> Date::sm_totalDaysOfNormalYearMonth{31, 28, 31, 30, 31, 30,
                                                                          31, 31, 30, 31, 30, 31};

class MonthPrinter
{
public:
    MonthPrinter(unsigned year, unsigned month) : m_year(year), m_month(month)
    {
        m_outputStartDay = sm_monthStartDay;
        switch (m_month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12: m_outputEndDay = 31; break;
        case 4:
        case 6:
        case 9:
        case 11: m_outputEndDay = 30; break;
        case 2: m_outputEndDay = Date::isLeapYear(m_year) ? 29 : 28; break;
        default: break;
        }
    }

    /// <summary>
    /// Centrally display the month name.
    /// </summary>
    void printMonthName()
    {
        const string& monthName = sm_monthDisplayName[m_month - 1];
        unsigned size = static_cast<unsigned>(monthName.length());

        unsigned prefixSpacesLength, suffixSpacesLength;
        if ((sm_lineMaxLength - size) % 2 == 0) {
            prefixSpacesLength = suffixSpacesLength = (sm_lineMaxLength - size) >> 1;
        } else {
            suffixSpacesLength = (sm_lineMaxLength - size) >> 1;
            prefixSpacesLength = suffixSpacesLength + 1;
        }

        for (unsigned i = 0; i < prefixSpacesLength; i++) cout << ' ';
        cout << monthName;
        for (unsigned i = 0; i < suffixSpacesLength; i++) cout << ' ';
    }

    /// <summary>
    /// Print week header.
    /// </summary>
    void printWeekName() { cout << sm_weekHeader; }

    /// <summary>
    /// Print date numbers line by line when call this function.
    /// </summary>
    /// <returns>true if any number was printed.</returns>
    bool printDates()
    {
        if (m_outputStartDay > m_outputEndDay) return false;

        if (m_outputStartDay == sm_monthStartDay) {
            // The first line
            Date currentDate(m_year, m_month, 1);
            unsigned spaces = (currentDate - sm_standardDate) % 7;

            for (unsigned i = 0; i < spaces; i++) cout << sm_spacesLiteral;

            for (unsigned i = 0; i < sm_lineMaxCount - spaces; i++) printf("%3d", m_outputStartDay++);
        } else if (m_outputStartDay + 6 <= m_outputEndDay) {
            for (unsigned i = 0; i < sm_lineMaxCount; i++) printf("%3d", m_outputStartDay++);
        } else {
            // The last line
            unsigned spaces = sm_lineMaxCount - 1 - (m_outputEndDay - m_outputStartDay);

            for (unsigned i = 0; i < sm_lineMaxCount - spaces; i++) printf("%3d", m_outputStartDay++);

            for (unsigned i = 0; i < spaces; i++) cout << sm_spacesLiteral;
        }
        cout << ' ';

        return true;
    }

private:
    static array<const string, Date::MONTHS_OF_YEAR> sm_monthDisplayName;
    static const string sm_weekHeader;
    static const string sm_spacesLiteral;
    static const Date sm_standardDate;

    static constexpr unsigned sm_lineMaxLength = 22;
    static constexpr unsigned sm_lineMaxCount = 7;
    static constexpr unsigned sm_monthStartDay = 1;

    unsigned short m_outputStartDay;
    unsigned short m_outputEndDay;
    unsigned m_year;
    unsigned m_month;
};

array<const string, Date::MONTHS_OF_YEAR> MonthPrinter::sm_monthDisplayName{
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"};

const string MonthPrinter::sm_weekHeader(" Su Mo Tu We Th Fr Sa ");

const string MonthPrinter::sm_spacesLiteral("   ");

const Date MonthPrinter::sm_standardDate(1970, 2, 1);

class PrinterProxy
{
public:
    static void printMonth(unsigned year, unsigned month) { printMonthsVec(year, {month}); }

    static void printMonthsVec(unsigned year, const vector<unsigned>& months)
    {
        // Construct month printers
        vector<MonthPrinter> printers;
        unsigned size = static_cast<unsigned>(months.size());
        for (unsigned i = 0; i < size; i++) printers.emplace_back(year, months[i]);

        // Start printing
        for (unsigned i = 0; i < size; i++) printers[i].printMonthName();
        cout << endl;

        for (unsigned i = 0; i < size; i++) printers[i].printWeekName();
        cout << endl;

        bool status = true;
        while (status) {
            for (unsigned i = 0; i < size; i++) {
                status &= printers[i].printDates();
            }
            cout << endl;

            if (!status) break;
        }
        cout << endl;
    }
};

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "Invalid args." << endl;
        exit(EXIT_FAILURE);
    }

    unsigned long year = stoul(argv[1]);

    // Print 3 month in a row by default
    for (unsigned i = 0; i < 4; i++) {
        unsigned startMonth = i * 3 + 1;
        PrinterProxy::printMonthsVec(year, {startMonth, startMonth + 1, startMonth + 2});
    }
}