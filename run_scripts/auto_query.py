import random
import subprocess
import sys
import time

MAX_WAIT_TIME_SECONDS = 15
MAX_QUERY_TIMEOUT_SECONDS = 30

def send_query(webpack_name):
	cl_args = ['/root/webpack-mdns/run_scripts/mdns-controller', '--query', webpack_name]
	start_time = time.perf_counter()
	subprocess.run(cl_args, timeout=MAX_QUERY_TIMEOUT_SECONDS)
	end_time = time.perf_counter()

	# check that the query succeeded
	total_time = end_time - start_time
	if total_time > MAX_QUERY_TIMEOUT_SECONDS:
		print('query failed: webpack not found')
		sys.exit(2)
	return total_time

def main(argv):
	if len(argv) < 3:
		print('usage: python3 auto_query.py [# of queries to send] [wpack name(s)]')
		sys.exit(2)

	num_queries = int(argv[1])
	webpack_names = argv[2:]
	num_packs = len(webpack_names)

	total_time_sum = 0.0
	total_time_list = []
	for i in range(1, num_queries+1):
		# todo: replace this with a generator
		time.sleep(random.randrange(MAX_WAIT_TIME_SECONDS))
		runtime = send_query(webpack_names[i-1])#send_query(webpack_names[(num_packs - 1) % i])
		total_time_list.append(str(runtime))
		total_time_sum += runtime

	print('>>>> START TIME OUTPUT')
	for i in total_time_list:
		print(i)
	print('<<<< END TIME OUTPUT')
	print('Ran ' + str(num_queries) + ' queries.')
	print('Average time per query: ' + str(total_time_sum / num_queries) + ' s.')


if __name__ == '__main__':
	main(sys.argv)
