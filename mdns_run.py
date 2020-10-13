import subprocess
import sys

# SET THIS UP AS A PYTHON SCRIPT TO RUN IN THE DOCKER CONTAINER
# tmux new-session -d -s mdns
# tmux new-window -d -t '=mdns' -n listen
# tmux send-keys -t '=mdns:=listen' './mdns_controller --listen [webpacklist]'
# tmux new-window -d -t '=mdns' -n query
# tmux send-keys -t '=mdns:=query' 'python auto_query.py [#] [wpack names]'

def main(argv):
	if len(argv) < 4:
		print('usage: python3 mdns_run.py [# of queries to send] [wpack name(s) to query] [wpack names(s) in collection]')
		print('specify webpack names like this: a.wp,b.wp,c.wp and so on. do not use spaces')
		sys.exit(2)

	num_queries = int(argv[1])
	query_webpacks = argv[2].replace(',', ' ')
	query_command = '\'python3 auto_query.py ' + str(num_queries) + ' ' + query_webpacks + '\''
	listen_webpacks = argv[3].replace(',', ' ')
	listen_command = '\'./mdns_controller --listen ' + listen_webpacks + '\''


	start_session = ['tmux', 'new-session', '-d', '-s', 'mdns']
	listen_window_create = ['tmux', 'new-window', '-d', '-t', '\'=mdns\'', '-n', 'listen']
	query_window_create = ['tmux', 'new-window', '-d', '-t', '\'=mdns\'', '-n', 'query']
	listen_run = ['tmux', 'send-keys', '-t', '\'=mdns:=listen\'', listen_command]
	query_run = ['tmux', 'send-keys', '-t', '\'=mdns:=query\'', query_command]

	subprocess.run(start_session)
	subprocess.run(listen_window_create)
	subprocess.run(query_window_create)
	subprocess.run(listen_run)
	subprocess.run(query_run)


if __name__ == '__main__':
	main(sys.argv)
