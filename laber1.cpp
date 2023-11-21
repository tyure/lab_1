#include <iostream>
#include<windows.h>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

class TimeUnit {
public:
    int value;

    TimeUnit(int v) : value(v) {}

    virtual bool isValid() const {
        return (value >= 0);
    }
};

class Hour : public TimeUnit {
public:
    Hour(int h) : TimeUnit(h) {}

    bool isValid() const override {
        return (value >= 0 && value < 24);
    }
};

class Minute : public TimeUnit {
public:
    Minute(int m) : TimeUnit(m) {}

    bool isValid() const override {
        return (value >= 0 && value < 60);
    }
};

class Second : public TimeUnit {
public:
    Second(int s) : TimeUnit(s) {}

    bool isValid() const override {
        return (value >= 0 && value < 60);
    }
};

struct Date {
    int year;
    int month;
    int day;

    Date(int y, int m, int d) : year(y), month(m), day(d) {}

    bool isLeapYear() const {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    bool isValid() const {
        if (year < 0 || month < 1 || month > 12 || day < 1)
            return false;

        const int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        int maxDays = daysInMonth[month];

        if (month == 2 && isLeapYear())
            maxDays = 29;

        return (day <= maxDays);
    }
};

struct Time {
    Hour hours;
    Minute minutes;
    Second seconds;

    Time(Hour h, Minute m, Second s) : hours(h), minutes(m), seconds(s) {}

    bool isValid() const {
        return hours.isValid() && minutes.isValid() && seconds.isValid();
    }
};

struct DateTime {
    Date date;
    Time time;

    DateTime(Date d, Time t) : date(d), time(t) {}

    bool isValid() const {
        return date.isValid() && time.isValid();
    }

    void addTimeDifference(const Time& diff) {
        time.seconds.value += diff.seconds.value;
        time.minutes.value += diff.minutes.value;
        time.hours.value += diff.hours.value;

        if (time.seconds.value >= 60) {
            time.seconds.value -= 60;
            time.minutes.value++;
        }

        if (time.minutes.value >= 60) {
            time.minutes.value -= 60;
            time.hours.value++;
        }

        if (time.hours.value >= 24) {
            time.hours.value -= 24;
            date.day++;
        }

        while (!date.isValid()) {
            int daysInMonth = 31;

            if (date.month == 2) {
                daysInMonth = date.isLeapYear() ? 29 : 28;
            }
            else if (date.month == 4 || date.month == 6 || date.month == 9 || date.month == 11) {
                daysInMonth = 30;
            }

            if (date.day > daysInMonth) {
                date.day -= daysInMonth;
                date.month++;

                if (date.month > 12) {
                    date.month = 1;
                    date.year++;
                }
            }
        }
    }

    int calculateDayOfWeek() const {
        int day = date.day;
        int month = date.month;
        int year = date.year;

        int a = (14 - month) / 12;
        int y = year - a;
        int m = month + 12 * a - 2;

        int dayOfWeek = (day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12) % 7;

        dayOfWeek = (dayOfWeek + 7) % 7;

        return dayOfWeek;
    }

    Time calculateTimeDifference(const DateTime& other) const {
        int totalSeconds1 = date.day * 24 * 3600 + time.hours.value * 3600 + time.minutes.value * 60 + time.seconds.value;
        int totalSeconds2 = other.date.day * 24 * 3600 + other.time.hours.value * 3600 + other.time.minutes.value * 60 +
            other.time.seconds.value;

        int diffSeconds = totalSeconds2 - totalSeconds1;

        int diffHours = diffSeconds / 3600;
        int diffMinutes = (diffSeconds % 3600) / 60;
        int diffSecs = diffSeconds % 60;

        return Time(Hour(diffHours), Minute(diffMinutes), Second(diffSecs));
    }
};

class ListOperations {
public:
    vector<DateTime> dateTimeList;

    void addDateTime(const DateTime& dt) {
        dateTimeList.push_back(dt);
    }

    void sortByDate() {
        sort(dateTimeList.begin(), dateTimeList.end(), [](const DateTime& a, const DateTime& b) {
            if (a.date.year != b.date.year) {
                return a.date.year < b.date.year;
            }
        if (a.date.month != b.date.month) {
            return a.date.month < b.date.month;
        }
        return a.date.day < b.date.day;
            });
    }

    void sortByTime() {
        sort(dateTimeList.begin(), dateTimeList.end(), [](const DateTime& a, const DateTime& b) {
            if (a.time.hours.value != b.time.hours.value) {
                return a.time.hours.value < b.time.hours.value;
            }
        if (a.time.minutes.value != b.time.minutes.value) {
            return a.time.minutes.value < b.time.minutes.value;
        }
        return a.time.seconds.value < b.time.seconds.value;
            });
    }
};

template<typename T>
void insertionSort(vector<T>& arr) {
    for (int i = 1; i < arr.size(); ++i) {
        T key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }

        arr[j + 1] = key;
    }
}

template<typename T>
void quickSort(vector<T>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

template<typename T>
int partition(vector<T>& arr, int low, int high) {
    T pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; ++j) {
        if (arr[j] < pivot) {
            ++i;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[high]);
    return i + 1;
}

template<typename T>
void mergeSort(vector<T>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

template<typename T>
void merge(vector<T>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<T> L(n1);
    vector<T> R(n2);

    for (int i = 0; i < n1; ++i)
        L[i] = arr[left + i];

    for (int j = 0; j < n2; ++j)
        R[j] = arr[mid + 1 + j];

    int i = 0;
    int j = 0;
    int k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            ++i;
        }
        else {
            arr[k] = R[j];
            ++j;
        }
        ++k;
    }

    while (i < n1) {
        arr[k] = L[i];
        ++i;
        ++k;
    }

    while (j < n2) {
        arr[k] = R[j];
        ++j;
        ++k;
    }
}

int main() {

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    ListOperations listOps;

    cout << "Введіть дати та час. Для завершення введіть '0 0 0' для дати та часу." << endl;

    while (true) {
        Date date(1, 0, 0);
        Time time(Hour(0), Minute(1), Second(0));
        DateTime dateTime(date, time);

        cout << "Введіть день та місяць (ДД ММ): ";
        cin >> dateTime.date.day >> dateTime.date.month;

        if (dateTime.date.day == 0 && dateTime.date.month == 0) {
            cout << "Завершення введення." << endl;
            break;
        }

        if (dateTime.date.month < 1 || dateTime.date.month > 12) {
            cout << "Невірний місяць. Будь ласка, введіть правильний місяць (1-12) та спробуйте ще раз." << endl;
            continue;
        }

        const int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        int maxDays = daysInMonth[dateTime.date.month];

        if (dateTime.date.month == 2 && dateTime.date.isLeapYear())
            maxDays = 29;

        if (dateTime.date.day < 1 || dateTime.date.day > maxDays) {
            cout << "Невірний день. Будь ласка, введіть правильний день (1-" << maxDays << ") для обраного місяця та спробуйте ще раз."
                << endl;
            continue;
        }

        cout << "Введіть рік (РРРР): ";
        cin >> dateTime.date.year;

        cout << "Введіть час (ГГ ХХ СС): ";
        cin >> dateTime.time.hours.value >> dateTime.time.minutes.value >> dateTime.time.seconds.value;

        if (dateTime.date.day == 0 && dateTime.date.month == 0 && dateTime.date.year == 0 &&
            dateTime.time.hours.value == 0 && dateTime.time.minutes.value == 0 && dateTime.time.seconds.value == 0) {
            cout << "Завершення введення." << endl;
            break;
        }

        if (!dateTime.isValid()) {
            cout << "Невірна дата або час. Будь ласка, введіть правильну дату та час та спробуйте ще раз." << endl;
            continue;
        }

        int dayOfWeek = dateTime.calculateDayOfWeek();
        const char* daysOfWeek[] = { "Неділя", "Понеділок", "Вівторок", "Середа", "Четвер", "П'ятниця", "Субота" };
        cout << "День тижня: " << daysOfWeek[dayOfWeek] << endl;

        listOps.addDateTime(dateTime);
    }

    listOps.sortByDate();

    cout << "Відсортовано за датою:" << endl;
    for (const auto& dt : listOps.dateTimeList) {
        cout << dt.date.day << "-" << dt.date.month << "-" << dt.date.year << " ";
        cout << dt.time.hours.value << ":" << dt.time.minutes.value << ":" << dt.time.seconds.value << endl;
    }

    listOps.sortByTime();

    cout << "\nВідсортовано за часом:" << endl;
    for (const auto& dt : listOps.dateTimeList) {
        cout << dt.date.day << "-" << dt.date.month << "-" << dt.date.year << " ";
        cout << dt.time.hours.value << ":" << dt.time.minutes.value << ":" << dt.time.seconds.value << endl;
    }

    return 0;
}
