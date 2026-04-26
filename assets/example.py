#!/bin/python
num1 = [[5, 7], [5, 6], [8, 3], [10, 3], [10, 11], [12, 11], [12, 13], [5, 13], [5, 11], [7, 11], [7, 7]]
num2 = [[3, 6], [3, 4], [4, 3], [12, 3], [13, 4], [13, 7], [13, 8], [7, 11], [13, 11], [13, 13], [3, 13], [3, 10.5], [10, 7], [10, 5], [6, 5], [6, 6]]
num3 = [[3, 3], [12, 3], [13, 4], [13, 7], [12, 8], [13, 9], [13, 12], [12, 13], [3, 13], [3, 11], [10, 11], [10, 9], [6, 9], [6, 7], [10, 7], [10, 5], [3, 5]]
num4 = [[3, 7], [5, 3], [8, 3], [6, 7], [9, 7], [9, 3], [12, 3], [12, 7], [13, 7], [13, 9], [12, 9], [12, 13], [9, 13], [9, 9], [3, 9]]
num5 = [[3, 3], [13, 3], [13, 5], [6, 5], [6, 7], [12, 7], [13, 8], [13, 12], [12, 13], [3, 13], [3, 11], [10, 11], [10, 9], [3, 9]]
num6 = [[4, 3], [13, 3], [13, 5], [6, 5], [6, 7], [12, 7], [13, 8], [13, 12], [12, 13], [4, 13], [3, 12], [3, 4], [6, 9], [6, 11], [10, 11], [10, 9]]
num7 = [[3, 3], [13, 3], [13, 5], [9, 13], [6, 13], [10, 5], [3, 5]]
num8 = [[3, 4], [4, 3], [12, 3], [13, 4], [13, 7], [12, 8], [13, 9], [13, 12], [12, 13], [4, 13], [3, 12], [3, 9], [4, 8], [3, 7], [6, 5], [6, 7], [10, 7], [10, 5], [6, 9], [6, 11], [10, 11], [10, 9]]
chX = [[3, 5], [5, 3], [8, 6], [11, 3], [13, 5], [10, 8], [13, 11], [11, 13], [8, 10], [5, 13], [3, 11], [6, 8]]

def normalize(coords):
	return [[coord[0] / 16.0, coord[1] / 16.0] for coord in coords]
def format_float(x):
	s = str(x)

	# If scientific notation → return as-is
	if "e" in s or "E" in s:
		return s

	# If no decimal point → add ".0"
	if "." not in s:
		s += ".0"

	return s + "f"

print(f"{{{', '.join([f"{{{format_float(first)}, {format_float(second)}}}" for first, second in normalize(num1)])}}};")
print(f"{{{', '.join([f"{{{format_float(first)}, {format_float(second)}}}" for first, second in normalize(num2)])}}};")
print(f"{{{', '.join([f"{{{format_float(first)}, {format_float(second)}}}" for first, second in normalize(num3)])}}};")
print(f"{{{', '.join([f"{{{format_float(first)}, {format_float(second)}}}" for first, second in normalize(num4)])}}};")
print(f"{{{', '.join([f"{{{format_float(first)}, {format_float(second)}}}" for first, second in normalize(num5)])}}};")
print(f"{{{', '.join([f"{{{format_float(first)}, {format_float(second)}}}" for first, second in normalize(num6)])}}};")
print(f"{{{', '.join([f"{{{format_float(first)}, {format_float(second)}}}" for first, second in normalize(num7)])}}};")
print(f"{{{', '.join([f"{{{format_float(first)}, {format_float(second)}}}" for first, second in normalize(num8)])}}};")
print(f"{{{', '.join([f"{{{format_float(first)}, {format_float(second)}}}" for first, second in normalize(chX)])}}};")