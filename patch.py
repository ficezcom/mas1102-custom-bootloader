import sys

def patch_firmware(original_file, my_kernel_file, output_file):
    try:
        with open(original_file, 'rb') as f:
            original_data = bytearray(f.read())
            
        with open(my_kernel_file, 'rb') as f:
            my_kernel_data = f.read()

        print(f"[*] Orijinal size: {len(original_data)} byte")
        print(f"[*] My Kernel size: {len(my_kernel_data)} byte")


        vector_chunk = my_kernel_data[0x00:0x60]
        original_data[0x1000:0x1000+len(vector_chunk)] = vector_chunk
        print("[+] Vector table patched at 0x1000")

        if len(my_kernel_data) > 0x32A00:                    #For after serial payload, Safe region same address in kernel.ld
            main_code = my_kernel_data[0x32A00:]
            original_data[0x33A00:0x33A00+len(main_code)] = main_code #For after serial payload, Safe region same address+0x1000 in kernel.ld
            print(f"[+] Code patched at 0x2000  ({len(main_code)} byte).")
        else:
            print("[!] Warning, no data in kernel at 0x2000 adress")

        with open(output_file, 'wb') as f:
            f.write(original_data)
        
        print(f"[#] Done! New file: {output_file}")
        print(f"[#] Final size: {len(original_data)} (must unchanged)")

    except Exception as e:
        print(f"[!] Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python patch.py orjinal.bin my_kernel.bin target.bin")
    else:
        patch_firmware(sys.argv[1], sys.argv[2], sys.argv[3])