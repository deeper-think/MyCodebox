import os
import time

SERVER_PATHS=['219.233.31.72:8090/down','219.233.63.133:8090/down','219.233.31.74:8090/down']

#SERVER_PATHS=['58.216.23.36:8090/down','58.216.23.37:8090/down','58.216.23.38:8090/down']

CMD_LINE_TIMEOUT='--connect-timeout 10 -m 600'
CMD_LINE_OUTPUT='-w %{http_code},%{time_total} -o data 2>data'
RESULT_MPTCP='result_mptcp.log'

LOG_NAME='result.log'

FILES=['4K','16K','64K','1M','16M','64M']

ISOTIMEFORMAT='%Y-%m-%d %X'


output = open(LOG_NAME, 'a')

try:

	while 1:
		for file in FILES:
			for server_path in SERVER_PATHS:
				cmd="curl "+server_path+"/"+file+" "+CMD_LINE_TIMEOUT+" "+CMD_LINE_OUTPUT
				#print cmd
				result = os.popen(cmd).readlines()
				#print result
				output.write(time.strftime(ISOTIMEFORMAT,time.localtime())+': '+server_path+': '+file+': '+result[0]+'\n')
				output.flush()
finally:
	output.close()    
	

