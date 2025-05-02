# IBC Project: Door Sensors Monitoring System (ESP8266 + RemoteXY)

[![Status](https://img.shields.io/badge/Status-Completed-brightgreen.svg)]()

Sistem pemantauan pintu berasaskan ESP8266 untuk kursus Internet Based Computing (IBC). Projek ini menggunakan sensor gas (analog) dan sensor jarak (ultrasonik) untuk memantau persekitaran pintu. Kawalan dan pemantauan utama dilakukan melalui aplikasi mudah alih menggunakan **RemoteXY** melalui sambungan WiFi Access Point terus ke ESP8266. Sistem ini juga memaparkan status pada LCD I2C 20x4 dan menghantar output ke Serial Monitor untuk penyahpepijatan.

## Jadual Kandungan

*   [Gambaran Keseluruhan Projek](#gambaran-keseluruhan-projek)
*   [Ciri-ciri Utama](#ciri-ciri-utama)
*   [Keperluan Perkakasan](#keperluan-perkakasan)
*   [Keperluan Perisian & Pustaka](#keperluan-perisian--pustaka)
*   [Konfigurasi Rangkaian (RemoteXY)](#konfigurasi-rangkaian-remotexy)
*   [Antaramuka RemoteXY](#antaramuka-remotexy)
*   [Sambungan Pin](#sambungan-pin)
*   [Pemasangan & Persediaan](#pemasangan--persediaan)
*   [Penggunaan](#penggunaan)
*   [Logik Operasi](#logik-operasi)
*   [Status Projek](#status-projek)
*   [Penyumbang](#penyumbang)
*   [Lesen](#lesen)

## Gambaran Keseluruhan Projek

Projek ini mengubah ESP8266 menjadi titik akses WiFi (Access Point). Aplikasi RemoteXY pada telefon pintar bersambung terus ke WiFi ESP8266 ini. Melalui aplikasi:
1.  Pengguna boleh **mengaktifkan/menyahaktifkan** sistem utama menggunakan suis maya.
2.  Pengguna boleh **memantau** bacaan sensor gas (nilai analog mentah) dan sensor jarak (dalam cm).
3.  Pengguna boleh **melihat status** LED Hijau dan Merah secara maya.

Di pihak ESP8266:
1.  Ia membaca nilai sensor gas dan jarak secara berkala.
2.  Jika sistem **diaktifkan** melalui RemoteXY *dan* jarak objek berada dalam lingkungan **10cm hingga 100cm**, geganti (relay) akan diaktifkan (diasumsikan membuka kunci pintu), LED Hijau menyala, dan LED Merah padam.
3.  Jika sistem **tidak aktif** ATAU jarak berada di luar lingkungan tersebut, geganti dinyahaktifkan (pintu kekal/dikunci), LED Hijau padam, dan LED Merah menyala.
4.  Semua status penting dipaparkan pada LCD I2C 20x4 dan dicetak ke Serial Monitor.

## Ciri-ciri Utama

*   **Mod Titik Akses WiFi (WiFi AP Mode):** ESP8266 mencipta rangkaian WiFi sendiri untuk sambungan terus dari aplikasi RemoteXY.
*   **Kawalan & Pemantauan Jauh (RemoteXY):** Menggunakan aplikasi RemoteXY untuk mengawal suis utama dan memantau sensor serta status LED.
*   **Pengesanan Gas:** Membaca nilai analog mentah dari sensor gas (jenis perlu dikenal pasti semasa kalibrasi).
*   **Pengesanan Jarak:** Mengukur jarak menggunakan sensor ultrasonik (HC-SR04 atau serupa).
*   **Kawalan Geganti (Relay):** Mengaktifkan/menyahaktifkan geganti berdasarkan logik jarak dan status sistem dari RemoteXY.
*   **Penunjuk Status LED:** LED Hijau (menunjukkan pintu mungkin terbuka/aktif) dan LED Merah (menunjukkan pintu tertutup/sistem tidak aktif).
*   **Paparan LCD:** Skrin LCD I2C 20x4 untuk memaparkan status sistem, status pintu, bacaan jarak, dan bacaan gas.
*   **Output Serial:** Maklumat terperinci dan status untuk tujuan penyahpepijatan dihantar ke Serial Monitor (Baud rate: 115200).

## Keperluan Perkakasan

*   **Mikropengawal:** Papan ESP8266 (cth., NodeMCU, Wemos D1 Mini)
*   **Sensor Gas:** Sensor Gas Analog (cth., MQ-2, MQ-5, dsb. - sambung ke **A0**)
*   **Sensor Jarak:** Sensor Ultrasonik HC-SR04 (atau serupa) (TRIG ke **D5**, ECHO ke **D4**)
*   **Modul Geganti (Relay):** Modul geganti 5V yang serasi dengan ESP8266 (Input kawalan ke **D7**)
*   **LED:** 1x LED Hijau (ke **D0**), 1x LED Merah (ke **D8**) - gunakan perintang yang sesuai jika perlu.
*   **Paparan LCD:** Modul LCD I2C 20x4 (Alamat I2C: **0x27**, SDA ke **D2**, SCL ke **D1**)
*   **Bekalan Kuasa:** Bekalan kuasa 5V yang stabil untuk ESP8266 dan komponen lain (cth., melalui USB atau penyesuai kuasa).
*   **Wayar Penyambung:** Wayar jumper secukupnya.
*   **Breadboard (Pilihan):** Untuk prototaip.

## Keperluan Perisian & Pustaka

*   **IDE:** Arduino IDE (dengan sokongan papan ESP8266) atau PlatformIO.
*   **Papan ESP8266 Core:** Pastikan pakej papan ESP8266 dipasang dalam Arduino IDE (melalui Boards Manager).
*   **Pustaka Arduino:**
    *   `ESP8266WiFi.h` (biasanya sebahagian daripada ESP8266 Core)
    *   `RemoteXY.h` (Muat turun dari laman web RemoteXY atau Library Manager jika ada)
    *   `Wire.h` (Standard, untuk I2C)
    *   `LiquidCrystal_I2C.h` (Pasang melalui Arduino Library Manager - cari oleh F Malpartida atau versi lain yang serasi)

## Konfigurasi Rangkaian (RemoteXY)

Kod ini mengkonfigurasi ESP8266 untuk berfungsi sebagai Titik Akses WiFi (Access Point). Telefon pintar anda perlu bersambung ke rangkaian WiFi ini untuk menggunakan aplikasi RemoteXY:

*   **SSID Rangkaian WiFi:** `ibc project`
*   **Kata Laluan Rangkaian WiFi:** `12345678`
*   **Port Pelayan RemoteXY:** `6377`

## Antaramuka RemoteXY

Konfigurasi antaramuka RemoteXY ditakrifkan dalam tatasusunan `RemoteXY_CONF[]` dan struktur `RemoteXY`. Antaramuka yang ditakrifkan dalam kod ini termasuk (berdasarkan struktur `RemoteXY`):

1.  `switch_relay`: Suis untuk menghidupkan/mematikan sistem utama.
2.  `gas_sensor`: Paparan nilai untuk bacaan sensor gas (float).
3.  `led_green`: Penunjuk LED maya untuk status LED hijau fizikal.
4.  `led_red`: Penunjuk LED maya untuk status LED merah fizikal.
5.  `ultrasonic`: Paparan nilai untuk bacaan sensor jarak (float).

**Penting:** Anda perlu membina antaramuka yang sepadan dalam editor RemoteXY dan memuat turun aplikasi yang dijana, atau menggunakan kod sumber RemoteXY ini untuk mengkonfigurasi aplikasi mudah alih anda secara manual agar sepadan dengan konfigurasi ini.

## Sambungan Pin

Pastikan semua komponen disambungkan ke pin ESP8266 yang betul seperti yang ditakrifkan dalam kod:

*   Sensor Gas (Output Analog) -> `A0`
*   Sensor Jarak (TRIG) -> `D5`
*   Sensor Jarak (ECHO) -> `D4`
*   Modul Geganti (Pin Input/Signal) -> `D7`
*   LED Hijau (Anod) -> `D0` (Katod ke GND melalui perintang jika perlu)
*   LED Merah (Anod) -> `D8` (Katod ke GND melalui perintang jika perlu)
*   LCD I2C (SDA) -> `D2`
*   LCD I2C (SCL) -> `D1`
*   LCD I2C (VCC) -> 5V / 3.3V (bergantung pada modul)
*   LCD I2C (GND) -> GND

*(Nota: Pastikan semua komponen berkongsi ground (GND) yang sama dengan ESP8266)*

## Pemasangan & Persediaan

1.  **Klon Repositori:**
    ```bash
    git clone https://github.com/[Your-GitHub-Username]/ibc-project-door-sensors.git
    cd ibc-project-door-sensors
    ```
2.  **Sambungan Perkakasan:** Sambungkan semua komponen mengikut [Sambungan Pin](#sambungan-pin).
3.  **Pasang IDE & Papan:** Pastikan Arduino IDE/PlatformIO dipasang bersama sokongan papan ESP8266.
4.  **Pasang Pustaka:** Pasang semua pustaka yang disenaraikan dalam [Keperluan Perisian & Pustaka](#keperluan-perisian--pustaka) menggunakan Library Manager Arduino IDE.
5.  **Konfigurasi Kod (Jika Perlu):** Kod ini sepatutnya berfungsi seperti sedia ada, tetapi anda mungkin mahu melaraskan ambang jarak (`10.0` dan `100.0`) dalam fungsi `prosesLogikUtama()`.
6.  **Muat Naik Kod:** Pilih papan ESP8266 yang betul (cth., NodeMCU 1.0) dan port yang betul dalam IDE anda. Kompil dan muat naik kod ke ESP8266.

## Penggunaan

1.  **Hidupkan ESP8266:** Bekalkan kuasa kepada litar.
2.  **Sambung WiFi:** Pada telefon pintar anda, cari rangkaian WiFi bernama `ibc project` dan sambung menggunakan kata laluan `12345678`.
3.  **Buka Aplikasi RemoteXY:** Lancarkan aplikasi RemoteXY yang telah anda konfigurasikan/jana agar sepadan dengan `RemoteXY_CONF` dalam kod ini.
4.  **Sambung dalam Aplikasi:** Dalam aplikasi RemoteXY, sambung ke ESP8266 (biasanya melalui mod WiFi Point atau Client, bergantung pada konfigurasi aplikasi anda - untuk kod ini, ia sepatutnya WiFi Point).
5.  **Gunakan Antaramuka:**
    *   Gunakan suis maya (`switch_relay`) untuk mengaktifkan/menyahaktifkan sistem.
    *   Perhatikan paparan nilai gas dan jarak.
    *   Perhatikan penunjuk LED maya.
6.  **Perhatikan Output Fizikal:**
    *   LCD akan memaparkan status semasa.
    *   LED Hijau/Merah akan menyala mengikut logik.
    *   Geganti akan berbunyi klik apabila diaktifkan/dinyahaktifkan.
7.  **Monitor Serial (Pilihan):** Buka Serial Monitor dalam Arduino IDE (tetapkan baud rate kepada `115200`) untuk melihat output penyahpepijatan terperinci.

## Logik Operasi

*   **Sistem TIDAK AKTIF (Suis RemoteXY OFF):**
    *   Geganti: OFF (Pintu Kunci)
    *   LED Hijau: OFF
    *   LED Merah: ON
*   **Sistem AKTIF (Suis RemoteXY ON):**
    *   **DAN Jarak DALAM Lingkungan (10cm < Jarak < 100cm):**
        *   Geganti: ON (Pintu Terbuka)
        *   LED Hijau: ON
        *   LED Merah: OFF
    *   **DAN Jarak LUAR Lingkungan (<10cm atau >100cm atau Ralat Sensor):**
        *   Geganti: OFF (Pintu Kunci)
        *   LED Hijau: OFF
        *   LED Merah: ON

## Status Projek

*   Status Semasa: **[Completed]**

## Penyumbang

*   [iwanzack] - ([itswinzie](https://github.com/itswinzie))
