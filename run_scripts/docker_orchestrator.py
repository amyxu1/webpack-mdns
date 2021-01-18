import subprocess
import sys
import time

def main(argv):
	if len(argv) < 5:
		print('usage: python docker_orchestrator.py [num queries] [query webpack list] [listen webpack list] [timewait]')
		print('specify webpack names like this: a.wp,b.wp,c.wp and so on. do not use spaces')
		exit(2)

	num_queries = argv[1]
	query_webpacks = argv[2].replace(',', ' ')
	listen_webpacks = argv[3].replace(',', ' ')
	time_wait = int(argv[4])

	subprocess.Popen('/root/webpack-mdns/run_scripts/mdns-controller --listen ' + listen_webpacks, shell=True)
	time.sleep(time_wait)
	subprocess.run('python3 /root/webpack-mdns/run_scripts/auto_query.py ' + num_queries + ' ' + query_webpacks, shell=True)
	time.sleep(20)


if __name__ == '__main__':
	main(sys.argv)
