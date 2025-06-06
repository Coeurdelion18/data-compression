import os
import struct

def convert_txt_to_raw_16bit(input_file_path, output_file_path):
    with open(input_file_path, 'r') as txt_file, open(output_file_path, 'wb') as raw_file:
        for line in txt_file:
            tokens = line.strip().split(',')
            for token in tokens:
                token = token.strip()
                if token:
                    try:
                        value = int(token)
                        # Clamp to int16 range just in case
                        if value < -32768 or value > 32767:
                            raise ValueError(f"Value {value} out of int16 range.")
                        raw_file.write(struct.pack('h', value))  # 'h' = 2-byte signed int
                    except ValueError as e:
                        print(f"Skipping invalid value in {input_file_path}: '{token}' ({e})")

def convert_all_txt_in_folder_16bit(input_dir, output_dir):
    os.makedirs(output_dir, exist_ok=True)
    
    for filename in os.listdir(input_dir):
        if filename.lower().endswith(".txt"):
            input_path = os.path.join(input_dir, filename)
            output_filename = os.path.splitext(filename)[0] + ".raw"
            output_path = os.path.join(output_dir, output_filename)
            convert_txt_to_raw_16bit(input_path, output_path)
            print(f"Converted: {filename} → {output_filename}")

if __name__ == "__main__":
    input_directory = "../Data/3_axis"       # Adjust your input directory here
    output_directory = "../Data/3_axis_raw"  # Adjust your output directory here
    convert_all_txt_in_folder_16bit(input_directory, output_directory)
    print("✅ All conversions complete.")
