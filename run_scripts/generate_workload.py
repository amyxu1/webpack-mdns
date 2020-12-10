import random

def main(argv):
	if len(argv) < 4:
		print('Usage: python3 generate_workload.py [total # queries] [# hosts] [file of webpacks]')
		sys.exit(2)

	with open(argv[3], 'r') as f:
		webpacks_list = f.readlines() 
	listen_webpacks_per_node = open(argv[1] + '_' + argv[2] + '_listen.csv', 'w+')
	query_webpacks_per_node = open(argv[1] + '_' + argv[2] + '_query.csv', 'w+')
	query_ct_per_node = open(argv[1] + '_' + argv[2] + '_query_ct.csv', 'w+')

	num_hosts = int(argv[2])
	num_queries = int(argv[1])
	num_queries_per_node = num_queries // num_hosts
	num_webpacks = len(webpacks_list)
	num_webpacks_assigned = 0
	for i in range(num_hosts):
		num_queries_to_select = num_queries_per_node
		if i == num_hosts - 1:
			num_queries_to_select = num_queries - (num_hosts - 1 * num_queries_per_node)

		# randomly select webpacks to query
		queries_for_node = random.sample(webpacks_list, num_queries_to_select)
		query_webpacks_per_node.write(','.join(queries_for_node))
		query_ct_per_node.write(num_queries_per_node)

		# sequentially assign webpacks
		remaining_webpacks = num_webpacks - num_webpacks_assigned
		num_to_assign = (remaining_webpacks // num_hosts) + (remaining_webpacks % num_hosts == 0)
		listen_webpacks_per_node.write(','.join(webpacks_list[i:i+num_to_assign]))
		num_webpacks_assigned -= num_to_assign

	listen_webpacks_per_node.close()
	query_webpacks_per_node.close()
	query_ct_per_node.close()

	listen_filename = argv[1] + '_' + argv[2] + '_listen.csv'
	query_filename = argv[1] + '_' + argv[2] + '_query.csv'
	query_ct_filename = argv[1] + '_' + argv[2] + '_query_ct.csv'
	print('To run workload, use this command:')
	print('python3 docker_setup.py ', str(num_hosts), query_ct_filename, query_filename, listen_filename, '[output_time_filename]')

