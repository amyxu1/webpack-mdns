import subprocess
import sys
import time
import netifaces as ni

def main(argv):
    if len(argv) < 5:
        print('usage: python docker_orchestrator.py [num queries] [query webpack list] [listen webpack list] [nic] [timewait]')
        print('specify webpack names like this: a.wp,b.wp,c.wp and so on. do not use spaces')
        exit(2)

    num_queries = argv[1]
    query_webpacks = argv[2].replace(',', ' ')
    listen_webpacks = argv[3].replace(',', ' ')
    time_wait = int(argv[4])
    nic = 'eth0'
    if len(argv) > 5:
        nic = argv[5]
    print(nic)
    #ip = ni.ifaddresses(nic)[ni.AF_INET][0]['addr']
    ip = argv[6]
    delay = '100'
    if len(argv) > 7:
        delay = argv[7]

    subprocess.run('tc qdisc add dev eth0 root netem delay ' + delay + 'ms', shell=True)
    subprocess.Popen('/root/webpack-mdns/run_scripts/mdns-controller --net ' + nic + ' --ip ' + ip + ' --listen ' + listen_webpacks, shell=True)
    time.sleep(time_wait)
    subprocess.run('python3 /root/webpack-mdns/run_scripts/auto_query.py '  + num_queries + ' ' + nic + ' '  + query_webpacks, shell=True)
    time.sleep(60)


if __name__ == '__main__':
    main(sys.argv)

