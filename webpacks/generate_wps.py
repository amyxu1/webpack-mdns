import random
import os
import sys

def main(argv):
    size = random.randint(1, 5) * 1000000
    start = int(argv[1])
    end = int(argv[2])
    for i in range(start, end+1):
        with open(str(i) + '.wp', 'wb+') as fout:
            fout.write(os.urandom(size)) 

if __name__ == '__main__':
    main(sys.argv)
