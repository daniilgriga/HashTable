import re
import sys
import os.path

def convert_to_binary (input_file, output_file):
    with open (input_file, 'r', encoding='utf-8') as f:
        text = f.read()

    words = re.findall (r'\w+', text)

    with open (output_file, 'wb') as f:
        for word in words:
            word_bytes = word.encode ('utf-8')

            if len (word_bytes) > 32:
                print (f"Warning: '{word}' longer than 32 bytes")
                word_bytes = word_bytes[:32]

            padded_bytes = word_bytes + b'\x00' * (32 - len(word_bytes))

            f.write (padded_bytes)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print ("Usage: python3 test/get_binary.py <input_file>")
        sys.exit(1)

    input_file = sys.argv[1]

    base, _ = os.path.splitext(input_file)
    output_file = base + '.bin'

    convert_to_binary (input_file, output_file)
    print (f"File successfully converted to {output_file}")
