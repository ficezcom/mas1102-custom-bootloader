import serial
import struct
import subprocess
ser = serial.Serial('COM13', 115200, timeout=1)

def send_command(addr, size, data, cmd):
    # Paketi gönder (13 byte)
    packet = struct.pack('<IIBB', addr, size, data, cmd)
    ser.write(packet)
    
    # 1. Debug Mesajlarını Oku ve Ekrana Bas
    # ACK (0x06) gelene kadar hattan gelen her şeyi oku
    debug_buffer = b""
    while True:
        char = ser.read(1)
        if not char: # Timeout
            print("Hata: Cihazdan cevap gelmedi.")
            exit()
            return False
        
        if char == b'\x06': # ACK'yı yakaladık!
            # ACK'dan önce biriken bir şey varsa debug olarak bas
            #if debug_buffer:
            #    print(f"[DEBUG RAW] {debug_buffer.decode('ascii', errors='ignore').strip()}")
            return True
        else:
            debug_buffer += char
             #Opsiyonel: Satır satır basmak istersen
            #if char == b'\n':
            #    print(f"[DEBUG] {debug_buffer.decode('ascii', errors='ignore').strip()}")
            #    debug_buffer = b""

def write_mem(address, value):
    # cmd=0x02 (Write), size=1 (byte)
    # Paket: addr, size, data, cmd
    #print(f"Yazılıyor: {hex(address)} -> {hex(value)}")
    
    # send_command fonksiyonunu önceki temiz haliyle kullanıyoruz
    if send_command(address, 1, value, 0x02):
        #print("Yazma işlemi başarılı ve doğrulandı (ACK alındı).")
        return True
    else:
        print("Yazma hatası!")
        return False
        
def write_bulk(start_address, hex_string):
    """
    start_address: Yazılacak RAM adresi (örn: 0xE0001000)
    hex_string: "0F 14 A0 E3..." şeklinde string veya b'\x0f\x14...' şeklinde bytes
    """
   # Hex string'i temizle ve byte listesine çevir
    raw_data = bytes.fromhex(hex_string.replace(" ", ""))
    
    print(f"Toplam {len(raw_data)} byte yazılacak...")
    
    for i, byte_value in enumerate(raw_data):
        current_addr = start_address + i
        # Senin çalışan fonksiyonunu çağırıyoruz
        if not write_mem(current_addr, byte_value):
            print(f"Hata! {hex(current_addr)} adresinde yazma durduruldu.")
            return False
            
    print("Tüm payload başarıyla yüklendi.")
    return True

 
import os
import struct
import time

def write_bin_file(file_path, start_address, chunk_size=256):
    """
    file_path: .bin dosyasının yolu
    start_address: Yazmaya başlanacak başlangıç adresi
    chunk_size: Her seferinde gönderilecek byte miktarı (C tarafındaki buffer'a uygun olmalı)
    """
    if not os.path.exists(file_path):
        print("Dosya bulunamadı!")
        exit()

    file_size = os.path.getsize(file_path)
    print(f"Dosya boyutu: {file_size} byte. Yazma başlıyor...")

    with open(file_path, "rb") as f:
        bytes_sent = 0
        
        while bytes_sent < file_size:
            # 1. Parçayı oku
            data_chunk = f.read(chunk_size)
            actual_chunk_size = len(data_chunk)
            current_addr = start_address + bytes_sent

            # 2. Bulk Write Paket Başlığı (CMD: 0x04)
            # Format: Addr(4), Size(4), Data(1), CMD(1)
            header = struct.pack("<IIBB", current_addr, actual_chunk_size, 0x00, 0x04)
            
            # 3. Önce başlığı sonra veriyi gönder
            ser.write(header)
            ser.write(data_chunk)

            # 4. Onay bekle
            ack = ser.read(1)
            if ack == b'\x06':
                bytes_sent += actual_chunk_size
                # İlerleme Çubuğu Hesapla
                progress = (bytes_sent / file_size) * 100
                print(f"\rİlerleme: %{progress:.2f} [{bytes_sent}/{file_size}]", end="")
            else:
                print(f"\n{hex(current_addr)} adresinde hata oluştu!")
                break

    print("\nTransfer başarıyla tamamlandı.")

 
def read_mem(address, length):
    if send_command(address, length, 0, 0x03):
        # ACK (0x06) hattan temizlendi, şimdi tam length kadar ham veri oku
        return ser.read(length)
    return None




import time

def execute_ram_code(address):
    """
    RAM'e yazılan kodu çalıştırır ve sonrasındaki çıktıları dinler.
    """
    jump_addr = address
    print(f"\n[SİSTEM] Kod çalıştırılıyor: {hex(address)}")
    
    # 1. Jump komutunu (0x05) gönder
    if send_command(address, 0, 0, 0x05):
        print("[SİSTEM] Jump komutu iletildi\n" + "-"*40)
        #ser.close()
        #subprocess.run(["python", "dump.py"])
        # 2. UART hattını sonsuz/uzun süreli dinleme döngüsüne al
        try:
            while True:
                # Hattaki veriyi byte byte oku
                if ser.in_waiting > 0:
                    char = ser.read(1)
                    
                    # Gelen byte'ı hem ASCII hem Hex olarak görebilmek için:
                    try:
                        decoded = char.decode('ascii')
                        # Eğer basılabilir bir karakterse ekrana yaz
                        if decoded.isprintable() or decoded in "\r\n":
                            print(decoded, end='', flush=True)
                        else:
                            print(f"[{char.hex().upper()}]", end='', flush=True)
                    except:
                        print(f"[{char.hex().upper()}]", end='', flush=True)
                
                # Küçük bir bekleme (CPU'yu yormamak için)
                time.sleep(0.001)
                
        except KeyboardInterrupt:
            print("\n[SİSTEM] Dinleme kullanıcı tarafından durduruldu.")
            return True
    else:
        print("[HATA] Jump komutu onaylanmadı.")
        return False

def human_readable_dump(data, start_address=0xf0013000, bytes_per_line=16):
    if not data:
        print("Veri boş!")
        return

    for i in range(0, len(data), bytes_per_line):
        # 1. Satır başı adresi (0xf0013000 formatında)
        current_address = start_address + i
        
        # 2. O satıra ait chunk'ı al
        chunk = data[i : i + bytes_per_line]
        
        # 3. Hex kısmını oluştur
        hex_values = []
        for b in chunk:
            hex_values.append(f"{b:02x}")
            
        # Eğer satır 16 byte'tan azsa kalan kısmı '-' ile doldur
        while len(hex_values) < bytes_per_line:
            hex_values.append("--")
            
        # 4. ASCII kısmını oluştur (Okunabilir karakterler için)
        ascii_values = "".join(chr(b) if 32 <= b <= 126 else "." for b in chunk)
        
        # 5. Ekrana yazdır
        hex_str = " ".join(hex_values)
        print(f"0x{current_address:08x}  {hex_str} |  {ascii_values} |")




write_bin_file( "Downloads/kernel.bin", 0x00000000)
execute_ram_code(0x00000000)

exit()



target_ram_addr = 0xe00e4c74 
data = read_mem(target_ram_addr, 0x20)
if data:
    print(f"Before :\n{data.hex()}")  
exit()  


payload = "0f32a0e30128a0e3002183e5"    
if write_bulk(target_ram_addr, payload):
    data = read_mem(target_ram_addr, 0x20) 
    
    print(f"After:\n{data.hex()}")
    execute_ram_code(target_ram_addr)
exit() 




target_ram_addr = 0xffff2330  # *--- BROM jump_addr
execute_ram_code(target_ram_addr)
exit()






target_ram_addr = 0xE00FFEFF
check_data = read_mem(target_ram_addr, 300)
human_readable_dump(check_data, target_ram_addr);

if write_mem(target_ram_addr, 0xff):
    check_data = read_mem(target_ram_addr, 1)
    print(f"Kontrol okuması: {check_data.hex()}")
exit()







####################memory dump
target_ram_addr = 0xffff0000  # Başlangıç adresi (örnektir)
chunk_size = 0x1000         # Her seferinde okunacak miktar (4096 byte)
total_size = 0x10000        # Almak istediğin toplam dump boyutu (örneğin 64KB)

full_data = bytearray()     # Okunan verilerin toplanacağı yer

print(f"Dump işlemi başlatılıyor: {hex(target_ram_addr)} adresinden {hex(total_size)} byte okunacak...")

for i in range(0, total_size, chunk_size):
    # Mevcut okuma adresi
    current_addr = target_ram_addr + i
    
    # Veriyi oku
    data = read_mem(current_addr, chunk_size)
    
    if data:
        full_data.extend(data)
        print(f"Okundu: {hex(current_addr)} - Kalan: {total_size - (i + chunk_size)} byte")
    else:
        print(f"Hata: {hex(current_addr)} adresinde okuma başarısız oldu!")
        break

# Elde edilen veriyi dosyaya kaydetme
with open("memory_dump.bin", "wb") as f:
    f.write(full_data)

print("\nİşlem tamamlandı. 'memory_dump.bin' dosyasına kaydedildi.")

exit()
####################memory dump






