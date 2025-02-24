#!/system/bin/sh
# Checking ID shell
if [ "$(id -u)" -ne 2000 ]; then
    echo "[ Error |@UnixeID(Yeye)]"
    exit 1
fi
# Smart Notification
shell() {
    sor="$1"
    cmd notification post -S bigtext -t '♨️ Adaptif power' 'Tag' "$sor" > /dev/null 2>&1
}
# Style display Terminal
    echo
    echo "     ☆================================☆"
    echo
    echo "       ~ Description Adaptif power.... "
    echo
    echo "       - Author                 :  @UnixeID"
    echo "       - Version                :  2.0"
    echo "       - Release               :  17 - Feb - 2025"
    echo "       - Name Shell         :  Adaptif power"
    echo
    echo "    |_______________________________________|"
    echo "    \______________________________________/"
    echo
    echo "   Adaptif power for game "
    sleep 2
    echo
    echo
        # Ambil daftar paket
      package_list=$(pm list package | cut -f2 -d: | tr -d '\r' | xargs -n1)
     control=1
       while IFS= read -r gamelist || [ -n "$gamelist" ]; do
          line=$(echo "$gamelist" | tr -d '\r' | xargs)
              if [ -n "$line" ]; then
        if echo "$package_list" | grep -xq "$line"; then
            echo "  $control. $line"
            control=$((control + 1))
          else
            echo "Paket game '$line' tidak ditemukan."
              fi
                fi
            done < /sdcard/Apple/gamelist.txt
   # Buat kill proses in background
  pkill -f logcat
       # Buat varibale instlling and uninstalling script
  if [ "$1" = kill ]; then
        if pgrep -f main > /dev/null 2>&1; then
         echo "  Program is stopped in the backgurond "
         rm /data/local/tmp/main > /dev/null 2>&1
         pkill -f main > /dev/null 2>&1
         shell "Program is stopped in the backgurond" 
         pkill -f sh > /dev/null 2>&1       
     else
       echo "Porgam faild stop !"
   fi
  else
     if pgrep -f main > /dev/null 2>&1; then
       cp /sdcard/Apple/main /data/local/tmp
       chmod +x /data/local/tmp/main
      nohup /data/local/tmp/main > /dev/null 2>&1
    fi 
      sleep 2
        if ! pgrep -f main > /dev/null 2>&1; then
        echo " Program is running in the backgurond"
     else
          echo "Porgram faild running !"
     fi
  fi 