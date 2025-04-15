#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <string>
using namespace std;

void NFC(const char *message) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
             "cmd notification post -S bigtext -t \"♨️ Adaptif Power\" \"Tag\" \"%s\" > /dev/null 2>&1",
             message);
    system(cmd);
}

void adjust_ionice_high(const char *pid) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "chrt -f -p 99 %s", pid);
    system(cmd);

    system("settings delete global device_idle_constants");
       
    system("settings put global performance_mode true");
    system("cmd power set-fixed-performance-mode true");
    system("cmd power set-adaptive-power-saver-enabled false");
    system("cmd display set-match-content-frame-rate-pref 120");
    system("cmd power set-mode 0");
    
    system("cmd device_config interaction_jank_monitor/animation_frame_duration 8.3");
    system("eval \"$(seq 512|sed 's/^/logcat -G /;s/$/mb/')\" >/dev/null 2>&1");
    system("logcat --prune=\"~$(pgrep -f system_server)\"");
    system("logcat --prune=\"~$(pgrep -f tombstoned)\"");
    
    system("setprop debug.perf_cpu_time_max_percent 0");
    system("setprop security.perf_harden 1");
    system("setprop security.perf_harden 0");
    
    system("cmd settings put global angle_gl_driver_all_angle 1");
    system("cmd settings put global game_driver_all_apps 1");
    system("cmd settings put global updatable_driver_all_apps 1");
    
    system("for a in $(cmd settings list global | grep anim | grep -v 'remove_animations' | cut -f1 -d=); do cmd settings put global $a 0.31; done");
    
    system("cmd settings put secure long_press_timeout 90");
    system("cmd settings put secure multi_press_timeout 105");
    
    system("cmd activity memory-factor set CRITICAL");
    system("settings put global activity_manager_constants usage_stats_interaction_interval_post_s=0,service_usage_interaction_time=0,usage_stats_interaction_interval=0,service_usage_interaction_time_post_s=0,power_check_max_cpu_1=0,power_check_max_cpu_2=0,power_check_max_cpu_3=0,power_check_max_cpu_4=0,service_start_foreground_anr_delay_ms=0,service_start_foreground_timeout_ms=0,min_assoc_log_duration=0,network_access_timeout_ms=0,fg_to_bg_fgs_grace_duration=0,fgs_start_foreground_timeout=0,power_check_interval=0,max_phantom_processes=16,memory_info_throttle_time=0,content_provider_retain_time=0");
    
    system("dumpsys media.metrics --clear");
    system("cmd activity untrack-associations >/dev/null 2>&1");
    system("dumpsys binder_calls_stats --disable");
    system("dumpsys binder_calls_stats --disable-detailed-tracking > /dev/null 2>&1");
    
    system("settings put global binder_calls_stats sampling_interval=600000000,detailed_tracking=disable,enabled=false,upload_data=false");

    snprintf(cmd, sizeof(cmd), "cmd deviceidle whitelist +%s", pid);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "cmd deviceidle except-idle-whitelist +%s", pid);
    system(cmd);
}

void adjust_ionice_low(const char *pid) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "chrt -f -p 15 %s", pid);
    system(cmd);

    system("settings put global device_idle_constants light_after_inactive_to=0,light_pre_idle_to=5000,light_idle_to=3600000,light_max_idle_to=43200000,locating_to=5000,location_accuracy=1000,inactive_to=0,sensing_to=0,motion_inactive_to=0,idle_after_inactive_to=0,idle_to=21600000,max_idle_to=172800000,quick_doze_delay_to=5000,min_time_to_alarm=300000");
    system("settings delete global activity_manager_constants");
    system("settings delete global binder_calls_stats");
    system("settings put performance_mode false");
    system("cmd power set-fixed-performance-mode false");
    system("cmd power set-adaptive-power-saver-enabled true");
    system("cmd display set-match-content-frame-rate-pref 120");
    system("cmd device_config interaction_jank_monitor/animation_frame_duration 8.3");
    system("cmd power set-mode 1");
    system("logcat -G 64k");
    system("setprop security.perf_harden 1");
    system("setprop security.perf_harden 0");
    system("setprop debug.perf_cpu_time_max_percent 70");
    system("for a in $(cmd settings list global | grep anim | grep -v 'remove_animations' | cut -f1 -d=); do cmd settings put global $a 0.40; done");
    system("cmd settings put secure long_press_timeout 280");
    system("cmd settings put secure multi_press_timeout 300");
    system("cmd activity memory-factor set NORMAL");
    system("cmd deviceidle enable deep > /dev/null 2>&1");
    system("cmd deviceidle force-idle deep > /dev/null 2>&1");
    system("cmd deviceidle force-inactive > /dev/null 2>&1");
    system("cmd activity idle-maintenance > /dev/null 2>&1");
}

bool check_app_running(const char *app_name, char *pid_buf, size_t pid_buf_size) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "pgrep -f %s", app_name);
    FILE *fp = popen(cmd, "r");
    if (!fp) return false;

    if (fgets(pid_buf, pid_buf_size, fp)) {
        pid_buf[strcspn(pid_buf, "\n")] = '\0';
        pclose(fp);
        return true;
    }

    pclose(fp);
    return false;
}

int main() {
    NFC("♨️ Priority is running in the background");
    sleep(1);
    NFC("♨️ Power Priority game: By @UnixeID");
    sleep(1);
    /*    Let's join my smart members, so that I am more enthusiastic about making 
    projects that can help people dill 
    */
    system("cmd activity start -a android.intent.action.VIEW -d https://www.instagram.com/pai_calll?igsh=OGZnYmZ5OGdiMG9r >/dev/null 2>&1");
    sleep(1);
    
    const char *filepath = "/sdcard/X-Treme/gamelist.txt";
    std::string current_game = "";
    char current_pid[32] = "";

    while (1) {
        FILE *file = fopen(filepath, "r");
        if (!file) {
            static int error_count = 0;
            if (error_count == 0) fprintf(stderr, "File gamelist.txt tidak ditemukan!\n");
            error_count++;
            sleep(2);
            continue;
        }

        char line[256], new_pid[32];
        bool found = false;

        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = '\0';
            if (check_app_running(line, new_pid, sizeof(new_pid))) {
                found = true;
                if (current_game != line || strcmp(new_pid, current_pid) != 0) {
                    current_game = line;
                    strcpy(current_pid, new_pid);
                    adjust_ionice_high(current_pid);

                    char notify_msg[512];
                    snprintf(notify_msg, sizeof(notify_msg), "[ Mode High For game : %s ]", line);
                    NFC(notify_msg);
                }
                break;
            }
        }

        fclose(file);

        if (!found && !current_game.empty()) {
            NFC("[ Mode Low : game closed ]");
            adjust_ionice_low(current_pid);
            current_game.clear();
            current_pid[0] = '\0';
        }

        sleep(2);
    }
    return 0;
}

