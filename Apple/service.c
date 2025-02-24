#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Fungsi untuk mengirim notifikasi menggunakan NFC (Android shell)
void NFC(const char *message) {
    char notifie[1024]; // Perbesar buffer untuk menghindari pemotongan string
    snprintf(notifie, sizeof(notifie),
             "CPU=$(dumpsys hardware_properties | grep 'CPU temperatures' | awk -F'[][]' '{print $2/10 \"°C\"}') && "
             "BATTERY=$(dumpsys battery | grep temperature | cut -d: -f2 | awk '{print $1/100 \"°C\"}') && "
             "cmd notification post -S bigtext "
             "-t \"[ Battery : $BATTERY | Cipset : $CPU ]\" \"Tag\" \"%s\" > /dev/null 2>&1",
             message);
    system(notifie);
}

// Fungsi untuk mengatur mode performa tinggi
void adjust_ionice_high(const char *pid) {
    char command[512];

     // Priority Backgorund 
    snprintf(command, sizeof(command), "chrt -f -p 99 %s", pid);
    system(command);

    // delete to time settings deepsleep
    system("settings delete global device_idle_constants");
    
    // Power settings and CMD
    system("settings put global performance_mode true");
    system("cmd power set-fixed-performance-mode true");
    system("cmd power set-adaptive-power-saver-enabled false");
    system("cmd display set-match-content-frame-rate-pref 120");
    system("cmd power set-mode 0");
    
    // logcat set buffer size limit Readust CPU
    system("eval \"$(seq 512|sed 's/^/logcat -G /;s/$/mb/')\" >/dev/null 2>&1");
    system("logcat --prune=\"~$(pgrep -f system_server)\"");
    system("logcat --prune=\"~$(pgrep -f tombstoned)\"");
    
    // Setprop CPU time max and securty Harden
    system("setprop security.perf_harden 1");
    system("setprop security.perf_harden 0");
    system("setprop debug.perf_cpu_time_max_percent 0");

    // idle Mode or deepsleep
    snprintf(command, sizeof(command), "cmd deviceidle whitelist +%s", pid);
    system(command);
}

// Fungsi untuk mengatur mode hemat daya
void adjust_ionice_low(const char *pid) {
    char command[512];

    // Priority Backgorund
    snprintf(command, sizeof(command), "chrt -f -p 15 %s", pid);
    system(command);
    
    // settings to time deepsleep
    system("settings put global device_idle_constants light_after_inactive_to=0,light_pre_idle_to=5000,light_idle_to=3600000,light_max_idle_to=43200000,locating_to=5000,location_accuracy=1000,inactive_to=0,sensing_to=0,motion_inactive_to=0,idle_after_inactive_to=0,idle_to=21600000,max_idle_to=172800000,quick_doze_delay_to=5000,min_time_to_alarm=300000");

    // Power settings and CMD
    system("settings put performance_mode false");
    system("cmd power set-fixed-performance-mode false");
    system("cmd power set-adaptive-power-saver-enabled true");
    system("cmd display set-match-content-frame-rate-pref 120");
    system("cmd power set-mode 1");
    
    // logcet size buffer
    system("logcat -G 64k");
    
     // Setprop CPU time max and securty Harden
    system("setprop security.perf_harden 1");
    system("setprop security.perf_harden 0");
    system("setprop debug.perf_cpu_time_max_percent 70");

    // idle mode or deepsleep
    system("cmd deviceidle enable deep > /dev/null 2>&1");
    system("cmd deviceidle force-idle deep > /dev/null 2>&1");
    system("cmd deviceidle force-inactive > /dev/null 2>&1");
    system("cmd activity idle-maintenance > /dev/null 2>&1");
}

// Fungsi untuk mengecek apakah aplikasi sedang berjalan
int check_app_running(const char *app_name, char *pid) {
    char command[512];
    snprintf(command, sizeof(command), "pgrep -f %s", app_name);

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        return 0;
    }

    if (fgets(pid, 16, fp) != NULL) {
        pid[strcspn(pid, "\n")] = '\0';  // Hapus karakter newline dari PID
        pclose(fp);
        return 1;
    }

    pclose(fp);
    return 0;
}

int main() {
    NFC("♨️ Priority is running in the background");
    sleep(1);
    NFC("♨️ Power Priority game: By @UnixeID");
    sleep(1);

    const char *primary_path = "/sdcard/Apple/gamelist.txt";
    char line[1024];
    char app_in_running[1024] = "";
    char current_pid[16] = ""; // Menyimpan PID game yang sedang berjalan

    while (1) {
        FILE *file = fopen(primary_path, "r");

        if (file == NULL) {
            static int error_count = 0;
            if (error_count == 0) {
                printf("File gamelist.txt tidak ditemukan!\n");
            }
            error_count++;
            sleep(2);
            continue;
        }

        int app_found = 0;
        char new_pid[16] = "";

        while (fgets(line, sizeof(line), file) != NULL) {
            line[strcspn(line, "\n")] = '\0';

            if (check_app_running(line, new_pid)) {
                app_found = 1;

                if (strcmp(line, app_in_running) != 0 || strcmp(new_pid, current_pid) != 0) {
                    snprintf(app_in_running, sizeof(app_in_running), "%s", line);
                    snprintf(current_pid, sizeof(current_pid), "%s", new_pid);

                    adjust_ionice_high(current_pid);

                    char message[512];
                    snprintf(message, sizeof(message), "[ Mode High For game : %s ]", line);
                    NFC(message);
                }
                break;
            }
        }
        fclose(file);

        // Jika tidak ada game yang berjalan, turunkan prioritas
        if (!app_found && strlen(app_in_running) > 0) {
            NFC("[ Mode Low : game closed ]");
            adjust_ionice_low(current_pid);

            app_in_running[0] = '\0';  // Kosongkan game yang berjalan
            current_pid[0] = '\0';     // Kosongkan PID yang tersimpan
        }

        sleep(2);
    }

    return 0;
}