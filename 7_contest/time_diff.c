/*Реализуйте программу,
которая читает со стандартного потока ввода строки - временные метки в формате:

YYYY-MM-DD HH:mm

Где YYYY - год, MM - месяц, DD - день, HH - час в 24-часовом формате, mm - минуты.

Гарантируется, что ввод содержит не менее двух строк.
На стандартный поток вывода необходимо вывести интервалы времени в минутах,
прошедшие между соседними временными метками.

Не забывайте учитывать возможные переводы стрелок зимнего/летнего времени,
а также законодательные инициативы по отмене/введению перевода часов на летнее время.*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>



const int32_t k_date_size = 19,
                k_month_offset = 5,
                k_day_offset = 8,
                k_hours_offset = 11,
                k_minutes_offset = 14;

void parse_date(char *date, struct tm *tm_date) {
    int32_t year, month, day, hours, minutes;

    // Divide date on segments.
    date[k_month_offset - 1] = 0;
    date[k_day_offset - 1] = 0;
    date[k_hours_offset - 1] = 0;
    date[k_minutes_offset - 1] = 0;
    date[k_minutes_offset + 2] = 0;
    /*for (int32_t i = 0; i < k_date_size; ++i) {
        printf("%d ", date[i]);
    }*/

    year = atoi(date);
    month = atoi(date + k_month_offset);
    day = atoi(date + k_day_offset);
    hours = atoi(date + k_hours_offset);
    minutes = atoi(date + k_minutes_offset);
    // printf("%d %d %d %d %d\n", year, month, day, hours, minutes);

    tm_date->tm_year = year - 1900;
    tm_date->tm_mon = month - 1;
    tm_date->tm_mday = day;
    tm_date->tm_hour = hours;
    tm_date->tm_min = minutes;
    tm_date->tm_sec = 0;
    tm_date->tm_isdst = -1;
}

time_t count_diff(char *date1, char *date2) {
    struct tm tm_date1, tm_date2;

    parse_date(date1, &tm_date1);
    parse_date(date2, &tm_date2);

    time_t seconds1 = mktime(&tm_date1), seconds2 = mktime(&tm_date2);
    // printf("\n%ld %ld\n", seconds1, seconds2);

    return difftime(seconds2, seconds1) / 60;
}

int32_t main(int32_t argc, char **argv) {
    char date1[k_date_size], date2[k_date_size];

    memset(date1, 0, k_date_size);
    memset(date2, 0, k_date_size);

    fgets(date1, k_date_size, stdin);
    while(fgets(date2, k_date_size, stdin) != NULL) {
        /*for (int32_t i = 0; i < k_date_size; ++i) {
            printf("%d ", date1[i]);
        }
        printf("\n");
        for (int32_t i = 0; i < k_date_size; ++i) {
            printf("%d ", date2[i]);
        }*/

        printf("%ld\n", count_diff(date1, date2));

        memset(date1, 0, k_date_size);
        for (int32_t i = 0; i < k_date_size; ++i) {
            date1[i] = date2[i];
        }
        memset(date2, 0, k_date_size);
    }

    //free(date1);
    //free(date2);

    return 0;
}
