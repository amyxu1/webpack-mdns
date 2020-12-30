import subprocess
import sys
import time

def main(argv):
	if len(argv) < 5:
		print('usage: python docker_orchestrator.py [num queries] [query webpack list] [listen webpack list] [timewait]')
		print('specify webpack names like this: a.wp,b.wp,c.wp and so on. do not use spaces')
		exit(2)

	num_queries = int(argv[1])
	query_webpacks = argv[2].replace(',', ' ')
	listen_webpacks = argv[3].replace(',', ' ')
	time_wait = int(argv[4])

	subprocess.run('/root/webpack-mdns/run_scripts/mdns-controller --listen ' + listen_webpacks)
	time.wait(time_wait)
	subprocess.run('/root/webpack-mdns/run_scripts/python3 auto_query.py ' + num_queries + ' ' + query_webpacks)


if __name__ == '__main__':
	main(sys.argv)
