import sys

def main(argv):
	if len(argv) < 3:
		print('usage: python parse_results.py [total result file name] [names of results files]')
		sys.exit(2)

	results_files = argv[2:]
	result = open(argv[1], 'w+')

	for result_file in results_files:
		with open(result_file) as f:
			lines = f.readlines()
		in_result = False

		for line in lines:
			if not in_result and line.startswith('>>>>'):
				in_result = True 
				continue

			if in_result:
				if line.startswith('<<<<'):
					in_result = False
					continue
				# may want to update in the future
				result.write(line)

	result.close()

if __name__ == '__main__':
	main(sys.argv)